#include <dory/core/registry.h>
#include <dory/core/services/enttSceneQueryService.h>
#include <dory/core/resources/scene/enttScene.h>
#include <dory/core/resources/scene/components.h>
#include <dory/core/resources/objects/transform.h>

namespace dory::core::services
{
    using namespace resources;
    using namespace resources::scene;
    using namespace resources::scene::components;
    using namespace repositories;

    EnttSceneQueryService::EnttSceneQueryService(Registry& registry) : DependencyResolver(registry)
    {}

    SceneState EnttSceneQueryService::getSceneState(const Scene& scene)
    {
        SceneState sceneState {};
        auto& enttScene = (EnttScene&)scene;

        auto enttView = enttScene.registry.view<Object, Mesh, Material, WorldTransform>();

        for (auto entity : enttView)
        {
            auto& object = enttView.get<Object>(entity);
            auto& mesh = enttView.get<Mesh>(entity);
            auto& material = enttView.get<Material>(entity);
            auto& transform = enttView.get<WorldTransform>(entity);

            auto visibleObject = VisibleObject {
                    object.id,
                    mesh.id,
                    material.id,
                    objects::Transform {
                            transform.position,
                            transform.orientation,
                            transform.scale
                    }
            };

            sceneState.objects[object.id] = visibleObject;
        }

        return sceneState;
    }
}