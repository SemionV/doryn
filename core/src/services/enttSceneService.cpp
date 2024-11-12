#include <dory/core/services/enttSceneService.h>
#include <dory/core/resources/scene/enttScene.h>
#include <dory/core/resources/scene/components.h>
#include <dory/core/resources/scene/enttComponents.h>

namespace dory::core::services
{
    namespace components = dory::core::resources::scene::components;

    resources::IdType EnttSceneService::addObject(resources::scene::Scene& scene, const resources::objects::SceneObject& object)
    {
        auto& enttScene = (resources::scene::EnttScene&)scene;
        auto& registry = enttScene.registry;

        auto entity = registry.create();
        auto id = ++enttScene.idCounter;
        enttScene.idMap[id] = entity;

        registry.emplace<components::Children>(entity);

        if(enttScene.idMap.contains(object.parentId))
        {
            auto parentEntity = enttScene.idMap[object.parentId];
            registry.emplace<components::Parent>(entity, parentEntity);

            if(registry.any_of<components::Children>(parentEntity))
            {
                auto& children = registry.get<components::Children>(parentEntity);
                children.entities.emplace_back(entity);
            }
        }

        registry.emplace<components::Name>(entity, object.name);
        registry.emplace<components::Position>(entity, glm::vec3{object.position.x, object.position.y, object.position.z});
        registry.emplace<components::Scale>(entity, glm::vec3{object.scale.x, object.scale.y, object.scale.z});
        registry.emplace<components::Mesh>(entity, object.meshId);

        return id;
    }

    void EnttSceneService::deleteObject(resources::scene::Scene& scene, resources::IdType objectId)
    {
    }
}