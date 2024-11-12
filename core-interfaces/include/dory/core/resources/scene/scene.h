#pragma once

#include "../entity.h"
#include "../ecsType.h"

namespace dory::core::resources::scene
{
    struct Scene: Entity<>
    {
        EcsType ecsType;
        std::string name {};
    };
}