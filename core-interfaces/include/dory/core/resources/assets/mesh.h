#pragma once

#include "../entity.h"
#include <vector>
#include <array>
#include <dory/math/linearAlgebra.h>

namespace dory::core::resources::assets
{
    struct Mesh: Entity<>
    {
        std::vector<math::Vector3f> points;
        std::vector<math::Vector2f> uvCoordinates;
        std::vector<math::Vector3f> colors;

        std::vector<std::array<std::size_t, 3>> triangles;
        std::vector<math::Vector3f> normals;
    };
}