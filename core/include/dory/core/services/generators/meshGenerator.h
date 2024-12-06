#pragma once

#include <dory/core/services/generators/iMeshGenerator.h>

namespace dory::core::services::generators
{
    class MeshGenerator: public IMeshGenerator
    {
    public:
        void cube(float width, resources::assets::Mesh& mesh) override;
    };
}
