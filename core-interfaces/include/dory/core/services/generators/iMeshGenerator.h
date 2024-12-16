#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/assets/mesh.h>

namespace dory::core::services::generators
{
    class IMeshGenerator: public generic::Interface
    {
    public:
        virtual void cube(float width, resources::assets::Mesh& mesh) = 0;
        virtual void rectangle(float width, float height, resources::assets::Mesh& mesh) = 0;
        virtual void triangle(float width, float height, resources::assets::Mesh& mesh) = 0;
    };
}
