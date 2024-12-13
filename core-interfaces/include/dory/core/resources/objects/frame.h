#pragma once

#include <dory/math/linearAlgebra.h>
#include <dory/core/resources/bindings/meshBinding.h>
#include <unordered_map>

namespace dory::core::resources::objects
{
    struct MeshItem
    {
        bindings::MeshBinding* meshBinding {};
        glm::mat4x4 modelTransform {};
    };

    struct Frame
    {
        math::Vector4f clearColor {};
        glm::mat4x4 viewProjectionTransform {};
        std::unordered_map<bindings::MaterialBinding*, std::vector<MeshItem>> meshMap {};
    };
}