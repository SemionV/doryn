#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <deque>
#include <dory/core/resources/assets/mesh.h>

#include <glm/glm.hpp>

#include <dory/math/geometryMorphing.h>

using namespace dory;
using namespace dory::core;
using namespace dory::core::resources;
using namespace dory::core::resources::assets;
using namespace dory::math;
using namespace dory::math::geometry;

template<typename T, auto N>
void assertPrism(const Shape<T, N> prism)
{
    EXPECT_EQ(prism.points.size() / N, 6);
}

TEST(Geometry, triangularPrism)
{
    auto width = 8.f;

    auto triangularPrismBase = Shape3d<float>{
            { {0, Point3f{ 0, width, -width, 0 }}, { 1, Point3f{ 1, width, 0, 0 }}, { 2, Point3f{2, 0, 0, 0 }} },
            { {0, Edge{ 0, 0, 1 }}, {1, Edge{ 1, 1, 2 }}, {2, Edge{ 2, 2, 0 }} },
            { {0, Face{ 0, { 0, 1, 2 } }} }
    };

    auto& face = triangularPrismBase.getFace(0);
    auto normal = glm::normalize(getNormal(face, triangularPrismBase));
    EXPECT_EQ(normal, glm::vec3(0, 0, 1));

    Shape3d<float> triangularPrism = ShapeMorphing::push(0, math::Vector3f{ 0, 0, -8 }, triangularPrismBase);
    EXPECT_EQ(triangularPrism.getFaceCount(), 5);
    /*auto& oppositeFace = triangularPrism.faces[1];
    EXPECT_EQ(oppositeFace.size(), 3);
    normal = glm::normalize(getNormal(oppositeFace[0], oppositeFace[1], triangularPrism));
    EXPECT_EQ(normal, glm::vec3(0, 0, -1));

    assertPrism(triangularPrism);*/
}