#include <dory/core/services/sceneService.h>

namespace dory::core::services
{
    void SceneService::addObject(resources::scene::Scene& scene, const resources::objects::SceneObject& object)
    {
        auto& objects = scene.objects;
        auto id = objects.id.size() + 1;

        objects.id.emplace_back(id);
        objects.name.emplace_back(object.name);
        objects.parent.emplace_back(object.parentId);
        objects.position.emplace_back(object.position);
        objects.scale.emplace_back(object.scale);
        objects.mesh.emplace_back(object.meshId);
        objects.idToIndex[id] = objects.id.size() - 1;

        if(object.parentId != resources::nullId)
        {
            auto index = objects.idToIndex[object.parentId];
            objects.children[index].emplace_back(id);
        }
    }
}