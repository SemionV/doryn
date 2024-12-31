#include <dory/core/registry.h>
#include <dory/core/controllers/movementController.h>

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

                            auto rotationView = registry.view<MovementAngularVelocity, Orientation>();
                            for (auto entity : rotationView)
                            {
                                auto& angularVelocity = rotationView.get<MovementAngularVelocity>(entity);
                                auto& orientation = rotationView.get<Orientation>(entity);

                                float angleSpeed = glm::length(angularVelocity.value);
                                glm::vec3 axis = glm::normalize(angularVelocity.value);

                                orientation.value = orientation.value * glm::angleAxis(angleSpeed * timeStep.ToSeconds(), axis);
                                orientation.value = glm::normalize(orientation.value);
                            }

                            auto movementView = registry.view<MovementLinearVelocity, Position>();
                            for (auto entity : movementView)
                            {
                                auto& position = movementView.get<Position>(entity);
                                auto& velocity = movementView.get<MovementLinearVelocity>(entity);
                                auto* distance = registry.try_get<MovementDistance>(entity);
                                auto* acceleration = registry.try_get<MovementAcceleration>(entity);

                                float timeStepSeconds = timeStep.ToSeconds();
                                float speed = velocity.value;
                                glm::vec3 direction = velocity.direction;

                                if(acceleration && acceleration->speed != 0.f)
                                {
                                    char accelerationSign = acceleration->speed / glm::abs(acceleration->speed);
                                    bool isThresholdReached = (float)accelerationSign * speed >= (float)accelerationSign * acceleration->targetSpeed;

                                    if(!isThresholdReached)
                                    {
                                        speed += acceleration->speed * timeStepSeconds;

                                        if((float)accelerationSign * speed > (float)accelerationSign * acceleration->targetSpeed)
                                        {
                                            speed = acceleration->targetSpeed;
                                        }

                                        velocity.value = speed;
                                    }
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

                                            //Translation is complete, remove the components
                                            registry.remove<MovementLinearVelocity>(entity);
                                            registry.remove<MovementDistance>(entity);
                                            if(acceleration)
                                            {
                                                registry.remove<MovementAcceleration>(entity);
                                            }
                                        }

                                        distance->done += step;

                                        float totalDistance = distance->left + distance->done;
                                        float d = distance->left / totalDistance;
                                        if(d <= 0.3f && acceleration && acceleration->speed > 0.f)
                                        {
                                            float a = -(speed * speed) / (2.0f * distance->left);
                                            acceleration->speed = a;
                                            acceleration->targetSpeed = 0.01f;
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