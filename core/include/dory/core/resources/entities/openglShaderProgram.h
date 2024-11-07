#pragma once

#include <dory/core/resources/entities/shaderProgram.h>

namespace dory::core::resources::entities
{
    struct OpenglShaderProgram: public ShaderProgram
    {
        GLuint index = GL_INVALID_INDEX;
    };
}

