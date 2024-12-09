#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <deque>
#include <dory/core/resources/assets/mesh.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace dory;
using namespace dory::core;
using namespace dory::core::resources;
using namespace dory::core::resources::assets;

template<typename TCoordinateType, std::size_t Dimensions>
struct Shape
{
    using FaceType = std::deque<std::size_t>;
    static constexpr std::size_t dimensions = Dimensions;

    std::vector<TCoordinateType> points;
    std::vector<std::size_t> edges;
    std::vector<FaceType> faces;
};

template<typename T>
using Shape3d = Shape<T, 3>;

struct Corner
{
    std::size_t left;
    std::size_t center;
    std::size_t right;
};

template<typename T>
std::size_t addPoint(glm::vec3 point, Shape3d<T>& shape)
{
    auto pointCount = shape.points.size() / Shape3d<T>::dimensions;

    shape.points.push_back(point.x);
    shape.points.push_back(point.y);
    shape.points.push_back(point.z);

    return pointCount;
}

template<typename T>
std::size_t addEdge(std::size_t pointA, std::size_t pointB, Shape3d<T>& shape)
{
    auto edgeCount = shape.edges.size() / 2;

    shape.edges.push_back(pointA);
    shape.edges.push_back(pointB);

    return edgeCount;
}

template<typename T>
glm::vec3 getPoint(std::size_t point, const Shape3d<T>& shape)
{
    std::size_t vertexIndex = point * 3;
    return glm::vec3{ shape.points[vertexIndex], shape.points[vertexIndex + 1], shape.points[vertexIndex + 2] };
}

template<typename T>
glm::vec3 getPoint(std::size_t edge, bool vertex, const Shape3d<T>& shape)
{
    std::size_t edgeIndex = edge * 2;
    return getPoint(shape.edges[edgeIndex + (unsigned char)vertex], shape);
}

template<typename T>
std::size_t getEdgeBegin(std::size_t edge, const Shape3d<T>& shape)
{
    return shape.edges[edge * 2];
}

template<typename T>
std::size_t getEdgeEnd(std::size_t edge, const Shape3d<T>& shape)
{
    return shape.edges[edge * 2 + 1];
}

template<typename T>
Corner getCorner(std::size_t edge1, std::size_t edge2, const Shape3d<T>& shape)
{
    Corner corner {};

    if(getEdgeBegin(edge1, shape) == getEdgeBegin(edge2, shape))
    {
        corner.left = getEdgeEnd(edge1, shape);
        corner.center = getEdgeBegin(edge1, shape);
        corner.right = getEdgeEnd(edge2, shape);
    }
    else if(getEdgeBegin(edge1, shape) == getEdgeEnd(edge2, shape))
    {
        corner.left = getEdgeEnd(edge1, shape);
        corner.center = getEdgeBegin(edge1, shape);
        corner.right = getEdgeBegin(edge2, shape);
    }
    else if(getEdgeEnd(edge1, shape) == getEdgeBegin(edge2, shape))
    {
        corner.left = getEdgeBegin(edge1, shape);
        corner.center = getEdgeEnd(edge1, shape);
        corner.right = getEdgeEnd(edge2, shape);
    }
    else if(getEdgeEnd(edge1, shape) == getEdgeEnd(edge2, shape))
    {
        corner.left = getEdgeBegin(edge1, shape);
        corner.center = getEdgeEnd(edge1, shape);
        corner.right = getEdgeBegin(edge2, shape);
    }

    return corner;
}

template<typename T>
glm::vec3 getNormal(std::size_t edge1, std::size_t edge2, const Shape3d<T>& shape)
{
    Corner corner = getCorner(edge1, edge2, shape);

    auto v1 = getPoint(corner.center, shape) - getPoint(corner.left, shape);
    auto v2 = getPoint(corner.center, shape) - getPoint(corner.right, shape);

    return glm::cross(v2, v1);
}

template<typename T>
Shape3d<T> push(const std::size_t faceId, glm::vec3 direction, const Shape3d<T>& shape)
{
    assert(faceId < shape.faces.size());

    Shape3d<T> resultShape {shape};

    auto& baseFace = shape.faces[faceId];
    auto& oppositeFace = resultShape.faces.emplace_back(typename Shape3d<T>::FaceType{});
    std::size_t i = 0;
    for(const auto& edge : baseFace)
    {
        std::size_t pointAd {};
        if(i == 0)
        {
            auto pointA = getPoint(edge, 0, shape);
            pointAd = addPoint(pointA + direction, resultShape);
        }
        else
        {
            //use end point of the previous edge, which is at index 0(reverse insertion order)
            pointAd = getEdgeEnd(oppositeFace[0], resultShape);
        }

        std::size_t pointBd {};
        if(i == baseFace.size() - 1)
        {
            //use begin point of the first edge, which is at the end of the collection
            pointBd = getEdgeBegin(oppositeFace[oppositeFace.size() - 1], resultShape);
        }
        else
        {
            auto pointB = getPoint(edge, 1, shape);
            pointBd = addPoint(pointB + direction, resultShape);
        }

        auto oppositeEdge = addEdge(pointAd, pointBd, resultShape);
        //insert opposite face edges are in reverse order, to point it's normal in opposite direction to base face
        oppositeFace.push_front(oppositeEdge);

        ++i;
    }

    return resultShape;
}

template<typename T>
Mesh buildMesh(const Shape3d<T>& shape)
{

}

TEST(Geometry, triangularPrism)
{
    auto width = 8.f;

    auto triangularPrismBase = Shape3d<float>{
            {width,-width,0, width,0,0, 0,0,0},
            {0,1, 1,2, 2,0},
            { {0, 1, 2} }
    };

    auto normal = glm::normalize(getNormal(0, 1, triangularPrismBase));
    EXPECT_EQ(normal, glm::vec3(0, 0, 1));

    normal = glm::normalize(getNormal(1, 2, triangularPrismBase));
    EXPECT_EQ(normal, glm::vec3(0, 0, 1));

    normal = glm::normalize(getNormal(2, 0, triangularPrismBase));
    EXPECT_EQ(normal, glm::vec3(0, 0, 1));

    auto triangularPrism = push(0, math::Vector3f{ 0, 0, -8 }, triangularPrismBase);
    EXPECT_EQ(triangularPrism.faces.size(), 2);
    auto& oppositeFace = triangularPrism.faces[1];
    EXPECT_EQ(oppositeFace.size(), 3);
    normal = glm::normalize(getNormal(oppositeFace[0], oppositeFace[1], triangularPrism));
    EXPECT_EQ(normal, glm::vec3(0, 0, -1));

    //auto mesh = buildMesh(triangularPrism);
}