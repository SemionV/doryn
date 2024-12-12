#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <deque>
#include <dory/core/resources/assets/mesh.h>

#include <glm/glm.hpp>

#include <dory/math/geometryMorphing.h>
#include "dory/math/tesselation.h"
#include <dory/core/meshGeneration.h>

using namespace dory;
using namespace dory::core;
using namespace dory::core::mesh;
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

    auto prismBase = Shape3d<float>{
            { {0, Point3f{ 0, width, -width, 0 }}, { 1, Point3f{ 1, width, 0, 0 }}, { 2, Point3f{2, 0, 0, 0 }} },
            { {0, Edge{ 0, 0, 1 }}, {1, Edge{ 1, 1, 2 }}, {2, Edge{ 2, 2, 0 }} },
            { {0, Face{ 0, { 0, 1, 2 } }} }
    };

    auto& face = prismBase.getFace(0);
    auto normal = glm::normalize(getNormal(face, prismBase));
    EXPECT_EQ(normal, glm::vec3(0, 0, 1));

    Shape3d<float> prism = ShapeMorphing::push(0, math::Vector3f{0, 0, -8 }, prismBase);
    EXPECT_EQ(prism.getFaceCount(), 5);

    math::geometry::Shape3df tessellatedPrism = Triangulator::tessellateFaceted(prism);

    Mesh mesh {};
    MeshGenerator::generate(tessellatedPrism, mesh);

    EXPECT_EQ(18, mesh.vertexCount);
}