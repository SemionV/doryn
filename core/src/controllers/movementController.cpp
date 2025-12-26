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
        _registry.getAll<repositories::ISceneRepository>([&](EcsType, repositories::ISceneRepository* repository) {
            repository->each([&](Scene& scene) {
                if(scene.ecsType ==EcsType::entt)
                {
                    auto& enttScene = (EnttScene&)scene;
                    auto& registry = enttScene.registry;

                    auto linearMovementView = registry.view<LinearMovement, Position, Object>();
                    for (auto entity : linearMovementView)
                    {
                        auto& movement = linearMovementView.get<LinearMovement>(entity);
                        auto& position = linearMovementView.get<Position>(entity);
                        auto& object = linearMovementView.get<Object>(entity);

                        if(movement.state.step > 0.f)
                        {
                            position.value += glm::normalize(movement.setup.value) * movement.state.step;

                            if(movement.state.distanceDone >= glm::length(movement.setup.value) && !movement.setup.endless)
                            {
                                auto eventDispatcher = _registry.get<events::scene::Bundle::IDispatcher>();
                                if(eventDispatcher)
                                {
                                    eventDispatcher->charge(events::scene::LinearMovementComplete{ scene.id, scene.ecsType, object.id });
                                }
                            }
                        }
                    }

                    auto rotationMovementView = registry.view<RotationMovement, Orientation, Object>();
                    for (auto entity : rotationMovementView)
                    {
                        auto& movement = rotationMovementView.get<RotationMovement>(entity);
                        auto& orientation = rotationMovementView.get<Orientation>(entity);
                        auto& object = linearMovementView.get<Object>(entity);

                        if(movement.state.step > 0.f)
                        {
                            orientation.value = orientation.value * glm::angleAxis(movement.state.step, glm::normalize(movement.setup.value));
                            orientation.value = glm::normalize(orientation.value);

                            if(movement.state.distanceDone >= glm::length(movement.setup.value) && !movement.setup.endless)
                            {
                                auto eventDispatcher = _registry.get<events::scene::Bundle::IDispatcher>();
                                if(eventDispatcher)
                                {
                                    eventDispatcher->charge(events::scene::RotationMovementComplete{ scene.id, scene.ecsType, object.id });
                                }
                            }
                        }
                    }
                }
            });
        });
    }
}