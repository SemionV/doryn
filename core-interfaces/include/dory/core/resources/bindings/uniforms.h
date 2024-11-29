#pragma once

#include "dory/math/linearAlgebra.h"
#include "../objects/materialProperties.h"

namespace dory::core::resources::bindings::uniforms
{
    struct Uniforms
    {
        math::Matrix4x4f modelTransform {};
        math::Matrix4x4f viewTransform {};
        math::Matrix4x4f projectionTransform {};
        objects::MaterialProperties material;
    };
}
