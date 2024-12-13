#pragma once

#include "dory/math/linearAlgebra.h"
#include "../objects/materialProperties.h"

namespace dory::core::resources::bindings::uniforms
{
    struct StaticUniforms
    {
        objects::MaterialProperties material;
    };

    struct DynamicUniforms
    {
        glm::mat4x4 viewProjectionTransform {};
    };

    struct ModelUniforms
    {
        glm::mat4x4 modelTransform {};
    };
}
