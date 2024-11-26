#pragma once

#include "materialBinding.h"

namespace dory::core::resources::bindings
{
    struct OpenglMaterialBinding: public MaterialBinding
    {
        GLuint glProgramId {};
    };
}