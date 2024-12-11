#pragma once

#include "geometry.h"

namespace dory::math::geometry
{
    class ShapeMorphing
    {
    public:
        static Shape3df push(std::size_t faceId, glm::vec3 direction, const Shape3df& shape);
    };
}
