#pragma once

#include <dory/core/resources/scene/scene.h>
#include <dory/core/resources/id.h>
#include <entt/entt.hpp>
#include <unordered_map>

namespace dory::core::resources::scene
{
    struct EnttScene: public Scene
    {
        entt::registry registry;
        std::unordered_map<resources::IdType, entt::entity> idMap;
        IdType idCounter {};

        EnttScene() = default;

        explicit EnttScene(const Scene& scene):
            Scene(scene)
        {}
    };
}


