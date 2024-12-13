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

        auto prismBase = Shape3d<float>{
                { {0, Point3f{ 0, -hw, hw, hw } }, { 1, Point3f{ 1, -hw, -hw, hw } }, { 2, Point3f{ 2, hw, -hw, hw} }, { 3, Point3f{ 3, hw, hw, hw} } },
                { { 0, Edge{ 0, 0, 1 } }, { 1, Edge{ 1, 1, 2 } }, { 2, Edge{ 2, 2, 3 } }, { 3, Edge{ 3, 3, 0 } } },
                { {0, Face{ 0, { 0, 1, 2, 3 } }} }
        };

        Shape3d<float> prism = ShapeMorphing::push(0, glm::vec3{0, 0, width }, prismBase);
        math::geometry::Shape3df tessellatedPrism = Triangulator::tessellateFaceted(prism);

        ShapeToMesh::generate(tessellatedPrism, mesh);
    }
}