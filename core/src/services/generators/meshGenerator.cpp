#include <dory/core/services/generators/meshGenerator.h>
#include "dory/core/meshGeneration.h"

namespace dory::core::services::generators
{
    template<typename T>
    struct Polygon
    {
        std::vector<T> points;
        std::vector<std::size_t> edges;
    };

    void MeshGenerator::cube(float width, resources::assets::Mesh& mesh)
    {
        mesh.vertexCount = 24;

        auto hw = width / 2;

        auto x = hw;
        auto y = hw;
        auto z = hw;

        mesh.positions.componentsCount = 3;
        mesh.positions.components = { -hw, hw, hw,  -hw, -hw, hw,  hw, -hw, hw,  hw, hw, hw,
                                      -hw, hw, hw,  -hw, -hw, hw,  hw, -hw, hw,  hw, hw, hw,
                                      -hw, hw, -hw,  -hw, -hw, -hw,  hw, -hw, -hw,  hw, hw, -hw};

        mesh.normals.componentsCount = 3;
        mesh.normals.components = { 0, 0, 1,  0, 0, 1,  0, 0, 1,  0, 0, 1 };
    }
}