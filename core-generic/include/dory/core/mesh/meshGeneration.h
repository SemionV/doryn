#pragma once

#include <optional>
#include "dory/math/geometry.h"
#include "dory/core/resources/assets/mesh.h"

namespace dory::core::mesh
{
    struct GenerationParameters
    {
        bool smoothNormals;
    };

    class MeshGenerator
    {
        template<typename T, auto N>
        static void generate(const math::geometry::Shape<T, N>& shape, resources::assets::Mesh& mesh, const GenerationParameters& parameters)
        {
            static_assert(N >= 2);

            mesh.positions.componentsCount = N;

            for(const auto& face : shape)
            {
                if(parameters.smoothNormals)
                {

                }
                else //distinct faces mode
                {
                    std::array<std::size_t, 3> triangleIndices {};
                    std::size_t vertexCount {};
                    for(const auto edgeId : face)
                    {
                        const auto& edge = shape.getEdge(edgeId);
                        const auto& pointA = shape.getPoint(edge.begin);
                        const auto& pointB = shape.getPoint(edge.end);

                        mesh.positions.components.push_back(pointA.x);
                        mesh.positions.components.push_back(pointA.y);
                        if constexpr (N > 2)
                        {
                            mesh.positions.components.push_back(pointA.z);
                        }

                        mesh.indices.push_back(mesh.vertexCount);

                        ++mesh.vertexCount;


                    }
                }
            }
        }
    };
}
