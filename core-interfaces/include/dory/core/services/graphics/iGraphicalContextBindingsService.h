#pragma once

#include "dory/generic/baseTypes.h"
#include <dory/core/resources/assets/mesh.h>
#include <dory/core/resources/bindings/meshBinding.h>

namespace dory::core::services::graphics
{
    class IGraphicalContextBindingsService: public generic::Interface
    {
    public:
        virtual void bindMesh(resources::assets::Mesh* mesh, resources::IdType sceneId) = 0;
        virtual resources::bindings::MeshBinding* getMeshBinding(resources::IdType meshId, resources::IdType graphicalContextBindingsId) = 0;
    };
}
