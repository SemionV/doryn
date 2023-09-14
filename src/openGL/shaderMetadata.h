#pragma once

#include "dependencies.h"

namespace dory::openGL
{
    struct ShaderMetadata
    {
        GLenum type;
        std::string identifier;
        std::string shaderSource;
        GLuint shaderId;
    };
}