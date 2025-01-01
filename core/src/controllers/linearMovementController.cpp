#include <dory/core/registry.h>
#include <dory/core/controllers/linearMovementController.h>

namespace dory::core::controllers
{
    using namespace core;
    using namespace resources;
    using namespace scene;
    using namespace components;

    LinearMovementController::LinearMovementController(core::Registry& registry):
            core::DependencyResolver{ registry }
    {}

    bool LinearMovementController::initialize(core::resources::IdType referenceId, core::resources::DataContext& context)
    {
        return true;
    }

    void LinearMovementController::stop(core::resources::IdType referenceId, core::resources::DataContext& context)
    {
    }

    void LinearMovementController::update(core::resources::IdType referenceId, const generic::model::TimeSpan& timeStep, core::resources::DataContext& context)
    {
        float timeStepSeconds = timeStep.ToSeconds();

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

                            auto view = registry.view<LinearMovement, Position>();
                            for (auto entity : view)
                            {
                                auto& position = view.get<Position>(entity);
                                auto& movement = view.get<LinearMovement>(entity);

                                float distanceTotal = glm::length(movement.translation);
                                float distanceLeft = distanceTotal - movement.distanceDone;

                                if(distanceLeft > 0.f)
                                {
                                    if(movement.acceleration > 0.f
                                       && movement.currentVelocity < movement.highVelocity
                                       && distanceLeft > movement.decelerationDistance)
                                    {
                                        movement.currentVelocity += movement.acceleration * timeStepSeconds;
                                        if(movement.currentVelocity > movement.highVelocity)
                                        {
                                            movement.currentVelocity = movement.highVelocity;
                                        }
                                    }
                                    else if(movement.deceleration < 0.f
                                            && movement.currentVelocity > movement.lowVelocity
                                            && distanceLeft <= movement.decelerationDistance)
                                    {
                                        movement.currentVelocity += movement.deceleration * timeStepSeconds;
                                        if(movement.currentVelocity < movement.lowVelocity)
                                        {
                                            movement.currentVelocity = movement.lowVelocity;
                                        }
                                    }

                                    if(movement.currentVelocity > 0.f)
                                    {
                                        auto step = movement.currentVelocity * timeStepSeconds;
                                        if(distanceLeft - step < 0.f)
                                        {
                                            step = distanceLeft;
                                        }

                                        movement.distanceDone += step;
                                        position.value = position.value + glm::normalize(movement.translation) * step;

                                        if(movement.distanceDone >= distanceTotal)
                                        {
                                            movement.currentVelocity = 0.f;
                                            movement.distanceDone = 0.f;
                                            movement.translation *= -1;

                                            //TODO: fire event about object's arrival to the destination of the linear movement
                                        }
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