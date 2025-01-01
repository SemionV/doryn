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
                        }
                    });
                }
            }
        });
    }
}