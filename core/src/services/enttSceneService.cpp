#include <dory/core/services/enttSceneService.h>
#include <dory/core/resources/scene/enttScene.h>
#include <dory/core/resources/scene/components.h>
#include <dory/core/resources/scene/enttComponents.h>
#include <stack>
#include <algorithm>

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