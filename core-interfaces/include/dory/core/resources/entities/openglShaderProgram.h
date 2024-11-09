#pragma once

#include "shaderProgram.h"

namespace dory::core::resources::entities
{
    struct OpenglShaderProgram: public ShaderProgram
    {
        GLuint index = GL_INVALID_INDEX;
    };
}

