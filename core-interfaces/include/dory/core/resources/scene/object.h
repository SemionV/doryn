#pragma once

#include "../id.h"
#include <vector>
#include <unordered_map>
#include <dory/math/linearAlgebra.h>

namespace dory::core::resources::scene
{
    struct Object
    {
        std::vector<IdType> id;
        std::vector<std::string> name {};
        std::vector<IdType> parent;
        std::vector<std::vector<IdType>> children;
        std::unordered_map<IdType, std::size_t> idToIndex;
        std::vector<math::Matrix4x4f> worldTransformation;
        std::vector<math::Vector3f> position;
        std::vector<math::Vector3f> scale;
        std::vector<IdType> mesh;
    };
}
