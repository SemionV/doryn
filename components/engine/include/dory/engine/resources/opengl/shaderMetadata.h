#pragma once

#include <string>

namespace dory::engine::resources::opengl
{
    struct ShaderMetadata
    {
        GLenum type;
        std::string identifier;
        std::string shaderSource;
        GLuint shaderId;
    };
}