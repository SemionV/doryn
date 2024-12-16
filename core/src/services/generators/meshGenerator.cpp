#include <dory/core/services/generators/meshGenerator.h>
#include <dory/math/geometryMorphing.h>
#include "dory/math/tesselation.h"
#include <dory/core/meshGeneration.h>

namespace dory::core::services::generators
{
    using namespace math;
    using namespace math::geometry;
    using namespace dory::core::mesh;
    using namespace dory::core::resources::assets;

    void MeshGenerator::cube(float width, Mesh& mesh)
    {
        auto hw = width / 2;

        auto shapeBase = Shape3d<float>{
                { {0, Point3f{ 0, -hw, hw, hw } }, { 1, Point3f{ 1, -hw, -hw, hw } }, { 2, Point3f{ 2, hw, -hw, hw} }, { 3, Point3f{ 3, hw, hw, hw} } },
                { { 0, Edge{ 0, 0, 1 } }, { 1, Edge{ 1, 1, 2 } }, { 2, Edge{ 2, 2, 3 } }, { 3, Edge{ 3, 3, 0 } } },
                { {0, Face{ 0, { 0, 1, 2, 3 } }} }
        };

        Shape3d<float> shape = ShapeMorphing::push(0, glm::vec3{0, 0, -width }, shapeBase);
        math::geometry::Shape3df tessellatedShape = Triangulator::tessellateFaceted(shape);

        ShapeToMesh::generate(tessellatedShape, mesh);
    }

    void MeshGenerator::rectangle(float width, float height, Mesh& mesh)
    {
        auto hw = width / 2;
        auto hh = width / 2;

        auto shape = Shape3d<float>{
                { {0, Point3f{ 0, -hw, hh, 0.f} }, { 1, Point3f{ 1, -hw, -hh, 0.f } }, { 2, Point3f{ 2, hw, -hh, 0.f} }, { 3, Point3f{ 3, hw, hh, 0.f} } },
                { { 0, Edge{ 0, 0, 1 } }, { 1, Edge{ 1, 1, 2 } }, { 2, Edge{ 2, 2, 3 } }, { 3, Edge{ 3, 3, 0 } } },
                { {0, Face{ 0, { 0, 1, 2, 3 } }} }
        };

        math::geometry::Shape3df tessellatedShape = Triangulator::tessellateFaceted(shape);
        ShapeToMesh::generate(tessellatedShape, mesh);
    }

    void MeshGenerator::triangle(float width, float height, Mesh& mesh)
    {
        auto hw = width / 2;

        auto shape = Shape3df{
                { {0, Point3f{ 0, -hw, 0, 0.f} }, { 1, Point3f{ 1, hw, 0, 0.f } }, { 2, Point3f{ 2, 0, height, 0.f} } },
                { { 0, Edge{ 0, 0, 1 } }, { 1, Edge{ 1, 1, 2 } }, { 2, Edge{ 2, 2, 0 } } },
                { {0, Face{ 0, { 0, 1, 2 } }} }
        };

        Shape3df tessellatedShape = Triangulator::tessellateFaceted(shape);
        ShapeToMesh::generate(tessellatedShape, mesh);
    }
}