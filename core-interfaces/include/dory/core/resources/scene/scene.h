#pragma once

#include "../entity.h"
#include "../ecsType.h"
#include "../objects/transform.h"
#include "../entities/view.h"
#include <string>
#include <memory>
#include <unordered_map>

namespace dory::core::resources::scene::configuration
{
    struct SceneConfiguration;
}

namespace dory::core::resources::scene
{
    struct Scene: Entity<>
    {
        std::string name {};
        EcsType ecsType {};
        IdType configurationId;
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
        std::unordered_map<IdType, VisibleObject> objects {};
    };

    struct SceneViewState
    {
        entities::View view;
        SceneState previous;
        SceneState current;
    };

    using SceneViewStateSet = std::unordered_map<resources::IdType, resources::scene::SceneViewState>;
}
