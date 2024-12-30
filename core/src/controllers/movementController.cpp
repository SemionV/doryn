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

                                float speed = glm::length(velocity.value);
                                if(speed > 0.f)
                                {
                                    glm::vec3 direction = glm::normalize(velocity.value);

                                    auto step = speed * timeStep.ToSeconds();
                                    if(distance) //adjust step according to travel distance
                                    {
                                        distance->left -= step;

                                        if(distance->left <= 0) //step over too far, reduce step to meet travel distance
                                        {
                                            step = step + distance->left; //step = step + distance - step = distance
                                            distance->left = 0.f;

                                            //Translation is complete, remove the components
                                            registry.remove<LinearVelocity>(entity);
                                            registry.remove<Distance>(entity);
                                        }

                                        distance->done += step;
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