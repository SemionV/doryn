#pragma once

#include <glad/gl.h>

namespace dory::renderer::opengl::resources
{
    struct Shader
    {
        GLuint id;
        GLenum type;
        std::string key;
        std::string sourceCode;

        Shader() = default;

        explicit Shader(GLenum type, std::string key):
                type(type), key(key)
        {}
    };
}
