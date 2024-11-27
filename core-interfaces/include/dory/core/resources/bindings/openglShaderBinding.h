#pragma once

#include "shaderBinding.h"

namespace dory::core::resources::bindings
{
    struct OpenglShaderBinding: public ShaderBinding
    {
        GLenum type;
        GLuint glId {};
    };
}