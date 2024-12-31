#include <dory/core/registry.h>
#include <dory/core/controllers/movementController.h>

#include <spdlog/fmt/fmt.h>

namespace dory::core::controllers
{
    using namespace core;
    using namespace resources;
    using namespace scene;
    using namespace components;

    MovementController::MovementController(core::Registry& registry):
        core::DependencyResolver{ registry }
    {}

    bool MovementController::initialize(core::resources::IdType referenceId, core::resources::DataContext& context)
    {
        return true;
    }

    void MovementController::stop(core::resources::IdType referenceId, core::resources::DataContext& context)
    {
    }

    void MovementController::update(core::resources::IdType referenceId, const generic::model::TimeSpan& timeStep, core::resources::DataContext& context)
    {
        auto logger = _registry.get<services::ILogService>();

        _registry.getAll<repositories::ISceneRepository, EcsType>([&](const auto& repos) {
            for(const auto& [key, value] : repos)
            {
                auto repoRef = value.lock();
                if(repoRef)
                {
                    repoRef->each([&](Scene& scene) {
                        if(scene.ecsType == core::resources::EcsType::entt)
                        {
                            auto& enttScene = (EnttScene&)scene;
                            auto& registry = enttScene.registry;

                            auto rotationView = registry.view<AngularVelocity, Orientation>();
                            for (auto entity : rotationView)
                            {
                                auto& angularVelocity = rotationView.get<AngularVelocity>(entity);
                                auto& orientation = rotationView.get<Orientation>(entity);

                                float angleSpeed = glm::length(angularVelocity.value);
                                glm::vec3 axis = glm::normalize(angularVelocity.value);

                                orientation.value = orientation.value * glm::angleAxis(angleSpeed * timeStep.ToSeconds(), axis);
                                orientation.value = glm::normalize(orientation.value);
                            }

                            auto movementView = registry.view<LinearVelocity, Position>();
                            for (auto entity : movementView)
                            {
                                auto& velocity = movementView.get<LinearVelocity>(entity);
                                auto& position = movementView.get<Position>(entity);
                                auto* distance = registry.try_get<Distance>(entity);
                                auto* deceleration = registry.try_get<Deceleration>(entity);

                                float timeStepSeconds = timeStep.ToSeconds();
                                float speed = glm::length(velocity.value);
                                float initialSpeed = glm::length(velocity.initialValue);
                                glm::vec3 direction = glm::normalize(velocity.value);

                                if(deceleration && speed > initialSpeed * deceleration->threshold)
                                {
                                    speed += deceleration->speed * timeStepSeconds;

                                    if(speed < initialSpeed * deceleration->threshold)
                                    {
                                        speed = initialSpeed * deceleration->threshold;
                                    }

                                    velocity.value = speed * direction;
                                }

                                if(speed > 0.f)
                                {
                                    auto step = speed * timeStepSeconds;

                                    if(distance) //adjust step according to travel distance
                                    {
                                        distance->left -= step;

                                        if(distance->left <= 0) //step over too far, reduce step to meet travel distance
                                        {
                                            step = step + distance->left; //step = step + distance - step = distance
                                            distance->left = 0.f;
                                            distance->done += step;

                                            //Translation is complete, remove the components
                                            //registry.remove<LinearVelocity>(entity);
                                            //registry.remove<Distance>(entity);
                                            if(deceleration)
                                            {
                                                registry.remove<Deceleration>(entity);
                                            }

                                            auto tempPosition = position.value + direction * step;
                                            if(logger)
                                            {
                                                logger->information(fmt::format("pos: [{0}, {1}, {2}]", tempPosition.x, tempPosition.y, tempPosition.z));
                                            }

                                            velocity.initialValue = -1.f * velocity.initialValue;
                                            velocity.value = velocity.initialValue;
                                            speed = glm::length(velocity.initialValue);

                                            distance->left = distance->done;
                                            distance->done = 0.f;
                                        }
                                        else
                                        {
                                            distance->done += step;
                                        }

                                        float totalDistance = distance->left + distance->done;
                                        float d = distance->left / totalDistance;
                                        if(d <= 1.f && !deceleration)
                                        {
                                            registry.emplace<Deceleration>(entity,
                                                Deceleration {
                                                    -(speed * speed) / (2.0f * distance->left),
                                                    0.1f
                                                });
                                        }
                                    }

                                    position.value = position.value + direction * step;
                                }
                            }
                        }
                    });
                }
            }
        });
    }
}