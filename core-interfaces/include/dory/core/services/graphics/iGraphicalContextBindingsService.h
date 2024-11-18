#pragma once

#include "dory/generic/baseTypes.h"
#include <dory/core/resources/assets/mesh.h>
#include <dory/core/resources/bindings/meshBinding.h>

namespace dory::core::services::graphics
{
    class IGraphicalContextBindingsService: public generic::Interface
    {
    public:
        virtual void bindMesh(resources::assets::Mesh* mesh) = 0;
        virtual resources::bindings::MeshBinding* getMeshBinding() = 0;
    };
}
