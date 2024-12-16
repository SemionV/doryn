#pragma once

#include <dory/core/services/generators/iMeshGenerator.h>

namespace dory::core::services::generators
{
    class MeshGenerator: public IMeshGenerator
    {
    public:
        void cube(float width, resources::assets::Mesh& mesh) override;
        void rectangle(float width, float height, resources::assets::Mesh& mesh) override;
        void triangle(float width, float height, resources::assets::Mesh& mesh) override;
    };
}
