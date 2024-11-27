#pragma once

#include "materialBinding.h"
#include <unordered_map>

namespace dory::core::resources::bindings
{
    struct OpenglMaterialBinding: public MaterialBinding
    {
        GLuint glProgramId {};
        std::unordered_map<std::size_t, GLint> uniformLocations {};
    };
}