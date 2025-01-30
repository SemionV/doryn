#include <dory/core/registry.h>
#include <dory/core/controllers/accelerationMovementController.h>
#include <chrono>

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
    void processAccelerationMovement(entt::registry& registry, std::chrono::duration<float> timeStepSeconds)
    {
        auto view = registry.view<T>();
        for (auto entity : view)
        {
            T& movement = view.template get<T>(entity);

            movement.state.step = 0.f;
            float distanceTotal = glm::length(movement.setup.value);
            float distanceLeft = distanceTotal - movement.state.distanceDone;

            if(distanceLeft > 0.f || movement.setup.endless)
            {
                if(movement.setup.acceleration > 0.f
                   && movement.state.currentVelocity < movement.setup.highVelocity
                   && distanceLeft > movement.setup.decelerationDistance)
                {
                    movement.state.currentVelocity += movement.setup.acceleration * timeStepSeconds.count();
                    if(movement.state.currentVelocity > movement.setup.highVelocity)
                    {
                        movement.state.currentVelocity = movement.setup.highVelocity;
                    }
                }
                else if(movement.setup.deceleration < 0.f
                        && movement.state.currentVelocity > movement.setup.lowVelocity
                        && distanceLeft <= movement.setup.decelerationDistance)
                {
                    movement.state.currentVelocity += movement.setup.deceleration * timeStepSeconds.count();
                    if(movement.state.currentVelocity < movement.setup.lowVelocity)
                    {
                        movement.state.currentVelocity = movement.setup.lowVelocity;
                    }
                }

                if(movement.state.currentVelocity > 0.f)
                {
                    auto step = movement.state.currentVelocity * timeStepSeconds.count();
                    if(distanceLeft - step < 0.f && !movement.setup.endless)
                    {
                        step = distanceLeft;
                    }

                    movement.state.distanceDone += step;
                    movement.state.step = step;
                }
            }
        }
    }

    void AccelerationMovementController::update(core::resources::IdType referenceId, const generic::model::TimeSpan& timeStep, core::resources::DataContext& context)
    {
        auto timeStepSeconds = std::chrono::duration_cast<std::chrono::duration<float>>(timeStep);

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