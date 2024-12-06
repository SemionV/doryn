#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <spdlog/fmt/fmt.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

template<typename T>
struct Polygon
{
    std::vector<T> points;
    std::vector<std::size_t> edges;
};

template<typename T>
glm::vec3 point(std::size_t point, const Polygon<T>& polygon)
{
    std::size_t vertexIndex = point * 3;
    return glm::vec3{ polygon.points[vertexIndex], polygon.points[vertexIndex + 1], polygon.points[vertexIndex + 2] };
}

template<typename T>
glm::vec3 point(std::size_t edge, unsigned char vertex, const Polygon<T>& polygon)
{
    std::size_t edgeIndex = edge * 2;
    return point(polygon.edges[edgeIndex + vertex], polygon);
}

template<typename T>
glm::vec3 vector(std::size_t edge, const Polygon<T>& polygon)
{
    return point(edge, 0, polygon) - point(edge, 1, polygon);
}

TEST(Geometry, triangularPrism)
{
    auto width = 8.f;

    auto basePolygon = Polygon<float>{
            {8,-8,0, 8,0,0, 0,0,0},
            {0,1, 1,2, 2,0}
    };

    basePolygon.points = {8,-8,0, 8,0,0, 0,0,0};
    basePolygon.edges = {0,1, 1,2, 2,0};

    auto v1 = vector(0, basePolygon);
    auto v2 = vector(1, basePolygon);

    auto normal = glm::normalize(glm::cross(v1, v2));

    EXPECT_EQ(normal.x, 0);
    EXPECT_EQ(normal.y, 0);
    EXPECT_EQ(normal.z, 1);
}