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

    SceneState EnttSceneQueryService::getSceneState(const resources::entities::View& view)
    {
        SceneState sceneState {};

        EnttScene* enttScene {};
        auto sceneRepo = _registry.get<ISceneRepository>(view.sceneEcsType);
        if(sceneRepo)
        {
            auto scene = sceneRepo->get(view.sceneId);
            if(scene)
            {
                enttScene = (EnttScene*)scene;
            }
        }

        if(enttScene)
        {
            auto enttView = enttScene->registry.view<Object, Mesh, Material, CombinedTransform>();

            for (auto entity : enttView)
            {
                auto& object = enttView.get<Object>(entity);
                auto& mesh = enttView.get<Mesh>(entity);
                auto& material = enttView.get<Material>(entity);
                auto& transform = enttView.get<CombinedTransform>(entity);

                auto visibleObject = VisibleObject {
                    object.id,
                    mesh.id,
                    material.id,
                    objects::Transform {
                        transform.position,
                        transform.rotation,
                        transform.scale
                    }
                };
                sceneState.objects[object.id] = visibleObject;
            }

            auto it = enttScene->idMap.find(view.cameraId);
            if(it != enttScene->idMap.end())
            {
                auto& viewTransform = enttScene->registry.get<CombinedTransform>(it->second);
                sceneState.viewTransform = objects::Transform{
                    viewTransform.position,
                    viewTransform.rotation,
                    viewTransform.scale
                };
            }
        }

        return sceneState;
    }
}