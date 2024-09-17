#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
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