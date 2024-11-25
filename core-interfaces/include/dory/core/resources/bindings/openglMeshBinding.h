#pragma once

#include "meshBinding.h"

namespace dory::core::resources::bindings
{
    struct OpenglMeshBinding: public MeshBinding
    {
        GLuint glVertexArrayId {};
    };
}
