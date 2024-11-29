#pragma once

#include "../entity.h"
#include "../objects/materialProperties.h"

namespace dory::core::resources::assets
{
    struct Material: Entity<>
    {
        std::vector<IdType> shaders {};
        objects::MaterialProperties properties;
    };
}