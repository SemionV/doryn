#pragma once

#include <dory/math/linearAlgebra.h>
#include <dory/core/resources/bindings/meshBinding.h>

namespace dory::core::resources::objects
{
    struct Frame
    {
        math::Vector4f clearColor {};
        std::vector<bindings::MeshBinding*> meshes {};
    };
}