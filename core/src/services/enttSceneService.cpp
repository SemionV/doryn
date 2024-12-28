#include <dory/core/services/enttSceneService.h>
#include <dory/core/resources/scene/components.h>
#include <dory/core/resources/scene/enttComponents.h>
#include <stack>
#include <algorithm>

namespace dory::core::services
{
    namespace components = dory::core::resources::scene::components;

    decltype(auto) EnttSceneService::createNewEntity(resources::scene::EnttScene& scene, resources::IdType& id)
    {
        auto entity = scene.registry.create();
        id = ++scene.idCounter;
        scene.idMap[id] = entity;

        return entity;
    }

    resources::IdType EnttSceneService::addObject(resources::scene::Scene& scene, const resources::objects::SceneObject& object)
    {
        auto& enttScene = (resources::scene::EnttScene&)scene;
        auto& registry = enttScene.registry;
        resources::IdType id;

        auto entity = createNewEntity(enttScene, id);

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
        registry.emplace<components::Object>(entity, id);
        registry.emplace<components::WorldTransform>(entity, object.transform);
        registry.emplace<components::Scale>(entity, object.transform.scale);
        registry.emplace<components::Orientation>(entity, object.transform.rotation);
        registry.emplace<components::Position>(entity, object.transform.position);

        return id;
    }

    void EnttSceneService::deleteObject(resources::scene::Scene& scene, resources::IdType objectId)
    {
        auto& enttScene = (resources::scene::EnttScene&)scene;
        auto& registry = enttScene.registry;

        if(enttScene.idMap.contains(objectId))
        {
            auto entity = enttScene.idMap[objectId];
            if(registry.valid(entity))
            {
                auto stack = std::stack<entt::entity>{};
                stack.push(entity);

                while(!stack.empty())
                {
                    auto currentEntity = stack.top();
                    stack.pop();

                    auto& children = registry.get<components::Children>(currentEntity);
                    for(const auto childEntity : children.entities)
                    {
                        stack.push(childEntity);
                    }

                    destroyObjectEntity(scene, currentEntity);

                    //cleanup id map
                    for(auto it = enttScene.idMap.begin(); it != enttScene.idMap.end(); ++it)
                    {
                        if(it->second == currentEntity)
                        {
                            enttScene.idMap.erase(it);
                            break;
                        }
                    }
                }
            }


            enttScene.idMap.erase(objectId);
        }
    }

    void EnttSceneService::destroyObjectEntity(resources::scene::Scene &scene, entt::entity entity)
    {
        auto& enttScene = (resources::scene::EnttScene&)scene;
        auto& registry = enttScene.registry;

        //delete from parent's children list
        if(registry.any_of<components::Parent>(entity))
        {
            auto& parentComponent = registry.get<components::Parent>(entity);
            if(registry.valid(parentComponent.entity) && registry.any_of<components::Children>(parentComponent.entity))
            {
                auto& children = registry.get<components::Children>(parentComponent.entity);
                auto it = std::find(children.entities.begin(), children.entities.end(), entity);
                if(it != children.entities.end())
                {
                    children.entities.erase(it);
                }
            }
        }

        registry.destroy(entity);
    }
}