#pragma once

#include "../entity.h"
#include "../ecsType.h"
#include "../objects/transform.h"
#include "components.h"
#include <string>

namespace dory::core::resources::scene
{
    struct Scene: Entity<>
    {
        std::string name {};
        EcsType ecsType {};
    };

    struct VisibleObject
    {
        IdType id {};
        IdType meshId {};
        IdType materialId {};
        objects::Transform transform {};
    };

    struct SceneState
    {
        objects::Transform viewTransform {};
        std::unordered_map<IdType, VisibleObject> objects {};
    };

    struct SceneViewState
    {
        IdType sceneId {};
        EcsType ecsType {};
        IdType windowId {};
        glm::mat4x4 projection {};
        SceneState previous;
        SceneState current;
        float alpha { 1 }; //Distance between previous and current state, 0 = previous state, 1 = current state, 0.5 = halfway between the states, etc
    };
}