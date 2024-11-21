#pragma once

#include <dory/generic/baseTypes.h>

namespace dory::core::services::graphics
{
    class IGpuDriver: public generic::Interface
    {
    public:
        virtual bool releaseMesh(core::resources::bindings::MeshBinding* meshBinding) = 0;
        virtual bool uploadMesh(core::resources::assets::Mesh* mesh, core::resources::bindings::MeshBinding* meshBinding) = 0;
    };
}
