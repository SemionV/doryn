#pragma once

#include "../entity.h"

namespace dory::core::resources::assets
{
    struct Material: Entity<>
    {
        math::Vector4f color {};
        std::vector<IdType> shaders {};
    };
}