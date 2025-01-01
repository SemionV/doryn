#include <dory/core/registry.h>
#include <dory/core/controllers/movementController.h>
#include <iostream>

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

                            auto linearMovementView = registry.view<LinearMovement, Position>();
                            for (auto entity : linearMovementView)
                            {
                                auto& movement = linearMovementView.get<LinearMovement>(entity);
                                auto& position = linearMovementView.get<Position>(entity);

                                if(movement.step > 0.f)
                                {
                                    position.value += glm::normalize(movement.value) * movement.step;

                                    //Motion is complete
                                    if(movement.distanceDone >= glm::length(movement.value))
                                    {
                                        movement.currentVelocity = 0.f;
                                        movement.distanceDone = 0.f;
                                        movement.value *= -1;

                                        //TODO: fire an event about object's arrival to the destination of the linear movement
                                    }
                                }
                            }

                            auto rotationMovementView = registry.view<RotationMovement, Orientation, Position>();
                            for (auto entity : rotationMovementView)
                            {
                                auto& movement = rotationMovementView.get<RotationMovement>(entity);
                                auto& orientation = rotationMovementView.get<Orientation>(entity);
                                auto& position = rotationMovementView.get<Position>(entity);

                                if(movement.step > 0.f)
                                {
                                    orientation.value = orientation.value * glm::angleAxis(movement.step, glm::normalize(movement.value));
                                    orientation.value = glm::normalize(orientation.value);

                                    if(movement.distanceDone >= glm::length(movement.value))
                                    {
                                        /*movement.currentVelocity = 0.f;
                                        movement.distanceDone = 0.f;
                                        movement.value *= -1;*/

                                        //TODO: fire an event about object's arrival to the destination of the linear movement
                                    }
                                }
                            }
                        }
                    });
                }
            }
        });
    }
}