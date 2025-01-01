#include <dory/core/registry.h>
#include <dory/core/controllers/accelerationMovementController.h>

namespace dory::core::controllers
{
    using namespace core;
    using namespace resources;
    using namespace scene;
    using namespace components;

    AccelerationMovementController::AccelerationMovementController(core::Registry& registry):
            core::DependencyResolver{ registry }
    {}

    bool AccelerationMovementController::initialize(core::resources::IdType referenceId, core::resources::DataContext& context)
    {
        return true;
    }

    void AccelerationMovementController::stop(core::resources::IdType referenceId, core::resources::DataContext& context)
    {
    }

    template<typename T>
    void processAccelerationMovement(entt::registry& registry, float timeStepSeconds)
    {
        auto view = registry.view<T>();
        for (auto entity : view)
        {
            AccelerationMovement& movement = view.template get<T>(entity);

            movement.step = 0.f;
            float distanceTotal = glm::length(movement.value);
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
                    movement.step = step;
                }
            }
        }
    }

    void AccelerationMovementController::update(core::resources::IdType referenceId, const generic::model::TimeSpan& timeStep, core::resources::DataContext& context)
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

                            processAccelerationMovement<LinearMovement>(registry, timeStepSeconds);
                            processAccelerationMovement<RotationMovement>(registry, timeStepSeconds);
                        }
                    });
                }
            }
        });
    }
}