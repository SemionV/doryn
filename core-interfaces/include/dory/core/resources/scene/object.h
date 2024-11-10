#pragma once

#include "../id.h"
#include <vector>
#include <dory/math/linearAlgebra.h>

namespace dory::core::resources::scene
{
    struct Object
    {
        std::vector<std::string> name {};
        std::vector<IdType> id;
        std::vector<IdType> parent;
        std::vector<std::vector<IdType>> children;
        std::vector<math::Vector3f> position;
        std::vector<math::Vector3f> scale;
        std::vector<math::Matrix4x4f> worldTransformation;
        std::vector<IdType> mesh;
    };
}
