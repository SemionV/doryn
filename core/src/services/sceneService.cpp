#include <dory/core/services/sceneService.h>
#include <algorithm>
#include <stack>

namespace dory::core::services
{
    resources::IdType SceneService::addObject(resources::scene::Scene& scene, const resources::objects::SceneObject& object)
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
        objects.children.emplace_back();

        if(object.parentId != resources::nullId)
        {
            auto index = objects.idToIndex[object.parentId];
            objects.children[index].emplace_back(id);
        }

        return id;
    }

    void SceneService::deleteObject(resources::scene::Scene& scene, resources::IdType objectId)
    {
        auto& objects = scene.objects;

        if(objects.idToIndex.contains(objectId))
        {
            auto stack = std::stack<resources::IdType>{};
            stack.push(objectId);

            while(!stack.empty())
            {
                auto id = stack.top();
                stack.pop();

                auto index = objects.idToIndex[id];

                cleanupSlot(scene, index);

                for(const auto childId : objects.children[index])
                {
                    stack.push(childId);
                }
            }
        }
    }

    void SceneService::cleanupSlot(resources::scene::Scene& scene, std::size_t slot)
    {
        auto& objects = scene.objects;

        auto objectId = objects.id[slot];
        objects.name[slot] = {};
        auto parentId = objects.parent[slot];
        if(parentId != resources::nullId)
        {
            auto parentIndex = objects.idToIndex[parentId];
            auto& children = objects.children[parentIndex];
            auto it = std::find(children.begin(), children.end(), objectId);
            if(it != children.end())
            {
                children.erase(it);
            }

            objects.parent[slot] = resources::nullId;
        }

        objects.worldTransformation[slot] = {};
        objects.position[slot] = {};
        objects.scale[slot] = {};
        objects.mesh[slot] = resources::nullId;

        objects.id[slot] = resources::nullId;
        objects.freeSlots.emplace_back(slot);
        objects.idToIndex.erase(objectId);
    }
}