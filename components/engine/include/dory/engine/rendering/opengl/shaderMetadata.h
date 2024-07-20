#pragma once

namespace dory::opengl
{
    struct ShaderMetadata
    {
        GLenum type;
        std::string identifier;
        std::string shaderSource;
        GLuint shaderId;
    };
}