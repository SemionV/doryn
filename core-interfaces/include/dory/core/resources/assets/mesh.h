#pragma once

#include "../entity.h"
#include <vector>
#include <array>
#include <dory/math/linearAlgebra.h>

namespace dory::core::resources::assets
{
    template<typename T>
    struct Vectors
    {
        using ComponentsType = T;
        std::size_t componentsCount {};
        std::vector<T> components;
    };

    struct Mesh: Entity<>
    {
        using IndexType = unsigned int;

        std::size_t vertexCount;
        Vectors<float> positions;
        Vectors<float> normals;
        Vectors<float> textureCoordinates;
        Vectors<float> colors;
        std::vector<IndexType> indices;
    };
}