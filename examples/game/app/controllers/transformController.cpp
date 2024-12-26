#include <dory/core/registry.h>
#include "transformController.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace dory::game
{
    using namespace core;
    using namespace resources;
    using namespace scene;
    using namespace components;

    TransformController::TransformController(core::Registry& registry):
        core::DependencyResolver{ registry }
    {}

    bool TransformController::initialize(core::resources::IdType referenceId, core::resources::DataContext& context)
    {
        return true;
    }

    void TransformController::stop(core::resources::IdType referenceId, core::resources::DataContext& context)
    {
    }

    void TransformController::update(core::resources::IdType referenceId, const generic::model::TimeSpan& timeStep, core::resources::DataContext& context)
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
                            auto view = enttScene.registry.view<CombinedTransform, Transform, Rotation>();

                            for (auto entity : view)
                            {
                                auto& combinedTransform = view.get<CombinedTransform>(entity);
                                auto& transform = view.get<Transform>(entity);
                                auto& rotation = view.get<Rotation>(entity);

                                combinedTransform.position = transform.position;
                                combinedTransform.rotation = glm::angleAxis(rotation.currentAngle, rotation.axis) * transform.rotation;
                                combinedTransform.scale = transform.scale;
                            }
                        }
                    });
                }
            }
        });
    }
}