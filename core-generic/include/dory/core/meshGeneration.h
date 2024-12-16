#pragma once

#include <optional>
#include "dory/math/geometry.h"
#include "dory/core/resources/assets/mesh.h"

namespace dory::core::mesh
{
    class ShapeToMesh
    {
    private:
        static void updateVertexNormal(resources::assets::Mesh& mesh, std::size_t vertexId, const glm::vec3& faceNormal)
        {
            std::size_t normalOffset = vertexId * mesh.normals.componentsCount;
            auto vertexNormal = glm::vec3{ mesh.normals.components[normalOffset], mesh.normals.components[normalOffset + 1], mesh.normals.components[normalOffset + 2] };
            vertexNormal += faceNormal;
            mesh.normals.components[normalOffset] = vertexNormal.x;
            mesh.normals.components[normalOffset + 1] = vertexNormal.y;
            mesh.normals.components[normalOffset + 2] = vertexNormal.z;
        }

        static void normalizeVertexNormal(resources::assets::Mesh& mesh, std::size_t vertexId)
        {
            std::size_t normalOffset = vertexId * mesh.normals.componentsCount;
            auto vertexNormal = glm::vec3{ mesh.normals.components[normalOffset], mesh.normals.components[normalOffset + 1], mesh.normals.components[normalOffset + 2] };
            vertexNormal = glm::normalize(vertexNormal);
            mesh.normals.components[normalOffset] = vertexNormal.x;
            mesh.normals.components[normalOffset + 1] = vertexNormal.y;
            mesh.normals.components[normalOffset + 2] = vertexNormal.z;
        }

    public:
        template<typename T, auto N>
        static void generate(const math::geometry::Shape<T, N>& shape, resources::assets::Mesh& mesh)
        {
            static_assert(N >= 2);

            mesh.positions.componentsCount = N;
            mesh.normals.componentsCount = 3;

            std::unordered_map<std::size_t, std::size_t> verticesMap {};

            for(const auto& [pointId, point] : shape.getPoints())
            {
                mesh.positions.components.emplace_back(point.x);
                mesh.positions.components.emplace_back(point.y);
                if constexpr (N > 2)
                {
                    mesh.positions.components.emplace_back(point.z);
                }
                if constexpr (N > 3)
                {
                    mesh.positions.components.emplace_back(1);
                }

                mesh.normals.components.emplace_back(0.f);
                mesh.normals.components.emplace_back(0.f);
                mesh.normals.components.emplace_back(0.f);

                verticesMap[pointId] = mesh.vertexCount++;
            }

            for(const auto& [faceId, face] : shape)
            {
                const math::geometry::Edge& edgeA = shape.getEdge(face.getEdge(0));
                const math::geometry::Edge& edgeB = shape.getEdge(face.getEdge(1));
                const math::geometry::Corner corner = math::geometry::getCorner(edgeA, edgeB);

                assert(verticesMap.contains(corner.left));
                assert(verticesMap.contains(corner.right));
                assert(verticesMap.contains(corner.center));

                auto leftIndex = verticesMap[corner.left];
                auto centerIndex = verticesMap[corner.center];
                auto rightIndex = verticesMap[corner.right];

                mesh.indices.emplace_back(leftIndex);
                mesh.indices.emplace_back(centerIndex);
                mesh.indices.emplace_back(rightIndex);

                glm::vec3 normal = glm::normalize(math::geometry::getNormal(face, shape));
                updateVertexNormal(mesh, leftIndex, normal);
                updateVertexNormal(mesh, centerIndex, normal);
                updateVertexNormal(mesh, rightIndex, normal);
            }

            for(std::size_t i = 0; i < mesh.vertexCount; ++i)
            {
                normalizeVertexNormal(mesh, i);
            }
        }
    };
}