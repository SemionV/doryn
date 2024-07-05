#pragma once

#include <dory/engine/rendering/opengl/shaderMetadata.h>
#include "blocks.h"

namespace dory::openGL::graphics
{
    struct Shader
    {
        GLuint id;
        GLenum type;
        std::string key;
        std::string sourceCode;

        Shader() = default;

        Shader(GLenum type, std::string key):
            type(type), key(key)
        {}
    };

    struct Program
    {
        GLuint id = unboundId;
        std::string key;
        std::vector<Shader> shaders;

        Program() = default;

        Program(std::string key, std::vector<Shader>&& shaders):
            key(key),
            shaders(std::move(shaders))
        {}
    };
}