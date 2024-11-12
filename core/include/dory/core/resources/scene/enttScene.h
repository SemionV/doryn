#pragma once

#include <dory/core/resources/scene/scene.h>
#include <entt/entt.hpp>

namespace dory::core::resources::scene
{
    struct EnttScene: public Scene
    {
        entt::registry registry;
    };
}


