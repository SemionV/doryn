#include <dory/core/registry.h>
#include "animationController.h"

namespace dory::game
{
    using namespace core;
    using namespace resources;
    using namespace scene;
    using namespace components;

    AnimationController::AnimationController(core::Registry& registry):
        core::DependencyResolver{ registry }
    {}

    bool AnimationController::initialize(core::resources::IdType referenceId, core::resources::DataContext& context)
    {
        return true;
    }

    void AnimationController::stop(core::resources::IdType referenceId, core::resources::DataContext& context)
    {
    }

    void AnimationController::update(core::resources::IdType referenceId, const generic::model::TimeSpan& timeStep, core::resources::DataContext& context)
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
                            auto view = enttScene.registry.view<Rotation>();

                            for (auto entity : view)
                            {
                                auto& rotation = view.get<Rotation>(entity);

                                float deltaAngle = rotation.angleSpeed * timeStep.ToSeconds();
                                rotation.currentAngle += deltaAngle;

                                rotation.currentAngle = std::fmod(rotation.currentAngle, glm::two_pi<float>());
                                if(rotation.currentAngle < 0.0f) {
                                    rotation.currentAngle += glm::two_pi<float>();
                                }
                            }
                        }
                    });
                }
            }
        });
    }
}