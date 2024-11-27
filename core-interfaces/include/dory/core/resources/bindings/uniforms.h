#pragma once

#include "dory/math/linearAlgebra.h"

namespace dory::core::resources::bindings::uniforms
{
    struct Material
    {
        math::Vector4f color;
        unsigned int meshId;
    };

    struct Uniforms
    {
        math::Matrix4x4f modelTransform;
        math::Matrix4x4f viewTransform;
        math::Matrix4x4f projectionTransform;
        Material material;
    };
}
