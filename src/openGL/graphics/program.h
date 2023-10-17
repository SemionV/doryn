#pragma once

#include "openGL/dependencies.h"
#include "openGL/shaderMetadata.h"
#include "blocks.h"

namespace dory::openGL::graphics
{
    struct Program
    {
        GLuint id = unboundId;
        std::vector<ShaderMetadata> shaders;

        Program() = default;

        Program(std::vector<ShaderMetadata>&& shaders):
            shaders(std::move(shaders))
        {}
    };
}