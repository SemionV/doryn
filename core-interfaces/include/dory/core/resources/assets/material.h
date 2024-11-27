#pragma once

#include "../entity.h"

namespace dory::core::resources::assets
{
    struct Material: Entity<>
    {
        std::vector<IdType> shaders {};
    };
}