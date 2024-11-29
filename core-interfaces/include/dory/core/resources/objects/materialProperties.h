#pragma once

#include <dory/math/linearAlgebra.h>

namespace dory::core::resources::objects
{
    struct MaterialProperties
    {
        math::Vector4f color {};
        math::Vector4f color2 {};
        math::Vector4f color3 {};
    };
}