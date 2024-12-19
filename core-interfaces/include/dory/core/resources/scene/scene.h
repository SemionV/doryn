#pragma once

#include "../entity.h"
#include "../ecsType.h"
#include "../objects/transform.h"
#include <string>

namespace dory::core::resources::scene
{
    struct Scene: Entity<>
    {
        EcsType ecsType {};
        std::string name {};
    };

    struct VisibleObject
    {
        objects::Transform localTransform;
        objects::Transform worldTransform;
        IdType meshId {};
    };
}