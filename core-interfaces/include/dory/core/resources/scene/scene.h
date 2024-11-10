#pragma once

#include "../entity.h"
#include "object.h"

namespace dory::core::resources::scene
{
    struct Scene: Entity<>
    {
        std::string name {};
        Object objects;
    };
}