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
        components::Transform localTransform {};
        components::Transform worldTransform {};
        IdType meshId {};
    };
}