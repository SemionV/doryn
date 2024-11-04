#pragma once

#include <glad/gl.h>
#include "id.h"

namespace dory::renderer::opengl::resources
{
    struct Shader
    {
        GLuint id = unboundId;
        GLuint programId {};
        GLenum type;
        std::string sourceCode {};

        Shader() = default;

        explicit Shader(GLenum type):
                type(type)
        {}
    };
}
