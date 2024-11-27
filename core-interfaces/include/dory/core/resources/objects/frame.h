#pragma once

#include <dory/math/linearAlgebra.h>
#include <dory/core/resources/bindings/meshBinding.h>
#include <unordered_map>

namespace dory::core::resources::objects
{
    struct Frame
    {
        math::Vector4f clearColor {};
        std::unordered_map<bindings::MaterialBinding*, std::vector<bindings::MeshBinding*>> meshMap {};
    };
}