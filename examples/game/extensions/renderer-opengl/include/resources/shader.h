#pragma once

#include <glad/gl.h>
#include <filesystem>
#include <string>
#include "id.h"

namespace dory::renderer::opengl::resources
{
    struct ShaderType
    {
        const constexpr static std::string_view fragmentShader = "openglFragmentShader";
        const constexpr static std::string_view vertexShader = "openglVertexShader";
    };

    struct Shader
    {
        GLuint id = unboundId;
        GLuint programId {};
        GLenum type {};
        std::string sourceCode {};
        std::filesystem::path filePath;

        Shader() = default;

        explicit Shader(GLenum type):
                type(type)
        {}
    };
}
