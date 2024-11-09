#pragma once

#include "shader.h"

namespace dory::core::resources::entities
{
    struct OpenglShader: public Shader
    {
        GLuint index = GL_INVALID_INDEX;
        GLuint programIndex {};
        GLenum glType {};
    };
}
