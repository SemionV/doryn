#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <spdlog/fmt/fmt.h>
#include <dory/math/linearAlgebra.h>
#include <dory/core/resources/assets/mesh.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace dory;
using namespace dory::core;
using namespace dory::core::resources;
using namespace dory::core::resources::assets;

template<typename TCoordinateType>
struct MeshDefinition
{
    std::vector<TCoordinateType> points;
    std::vector<std::size_t> edges;
    std::vector<std::vector<std::size_t>> faces;
};

template<typename T>
glm::vec3 point(std::size_t point, const MeshDefinition<T>& meshDefinition)
{
    std::size_t vertexIndex = point * 3;
    return glm::vec3{ meshDefinition.points[vertexIndex], meshDefinition.points[vertexIndex + 1], meshDefinition.points[vertexIndex + 2] };
}

template<typename T>
glm::vec3 point(std::size_t edge, unsigned char vertex, const MeshDefinition<T>& meshDefinition)
{
    std::size_t edgeIndex = edge * 2;
    return point(meshDefinition.edges[edgeIndex + vertex], meshDefinition);
}

template<typename T>
glm::vec3 faceNormal(std::size_t edge1, std::size_t edge2, const MeshDefinition<T>& meshDefinition)
{
    std::size_t edge1Index = edge1 * 2;
    std::size_t edge2Index = edge2 * 2;

    std::size_t points [3];

    if(meshDefinition.edges[edge1Index] == meshDefinition.edges[edge2Index])
    {
        points[0] = meshDefinition.edges[edge1Index + 1];
        points[1] = meshDefinition.edges[edge1Index];
        points[2] = meshDefinition.edges[edge2Index + 1];
    }
    else if(meshDefinition.edges[edge1Index] == meshDefinition.edges[edge2Index + 1])
    {
        points[0] = meshDefinition.edges[edge1Index + 1];
        points[1] = meshDefinition.edges[edge1Index];
        points[2] = meshDefinition.edges[edge2Index];
    }
    else if(meshDefinition.edges[edge1Index + 1] == meshDefinition.edges[edge2Index])
    {
        points[0] = meshDefinition.edges[edge1Index];
        points[1] = meshDefinition.edges[edge1Index + 1];
        points[2] = meshDefinition.edges[edge2Index + 1];
    }
    else if(meshDefinition.edges[edge1Index + 1] == meshDefinition.edges[edge2Index + 1])
    {
        points[0] = meshDefinition.edges[edge1Index];
        points[1] = meshDefinition.edges[edge1Index + 1];
        points[2] = meshDefinition.edges[edge2Index];
    }

    auto v1 = point(points[1], meshDefinition) - point(points[0], meshDefinition);
    auto v2 = point(points[1], meshDefinition) - point(points[2], meshDefinition);

    return glm::cross(v2, v1);
}

template<typename T>
MeshDefinition<T> push(const std::size_t face, math::Vector3f direction, const MeshDefinition<T>& mesh)
{

}

template<typename T>
Mesh buildMesh(const MeshDefinition<T>& meshDefinition)
{

}

TEST(Geometry, triangularPrism)
{
    auto width = 8.f;

    auto triangularPrismBase = MeshDefinition<float>{
            {width,-width,0, width,0,0, 0,0,0},
            {0,1, 1,2, 2,0},
            { {0, 1, 2} }
    };

    auto normal = glm::normalize(faceNormal(0, 1, triangularPrismBase));
    EXPECT_EQ(normal.x, 0);
    EXPECT_EQ(normal.y, 0);
    EXPECT_EQ(normal.z, 1);

    normal = glm::normalize(faceNormal(1, 2, triangularPrismBase));
    EXPECT_EQ(normal.x, 0);
    EXPECT_EQ(normal.y, 0);
    EXPECT_EQ(normal.z, 1);

    normal = glm::normalize(faceNormal(2, 0, triangularPrismBase));
    EXPECT_EQ(normal.x, 0);
    EXPECT_EQ(normal.y, 0);
    EXPECT_EQ(normal.z, 1);

    //auto triangularPrism = push(0, math::Vector3f{ 0, 0, -8 }, triangularPrismBase);
    //auto mesh = buildMesh(triangularPrism);
}