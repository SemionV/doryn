#pragma once

#include "../entity.h"
#include "../objects/materialProperties.h"

namespace dory::core::resources::assets
{
    enum class PolygonMode
    {
        Solid,
        Wireframe
    };

    struct Material: Entity<>
    {
        std::vector<IdType> shaders {};
        objects::MaterialProperties properties;
        PolygonMode polygonMode { PolygonMode::Solid };
    };
}