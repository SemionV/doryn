#pragma once

#include "openGL/dependencies.h"
#include "openGL/shaderMetadata.h"

namespace dory::openGL::graphics
{
    struct Program
    {
        static constexpr GLuint unboundProgramId = -1;
        GLuint id = unboundProgramId;
        std::vector<ShaderMetadata> shaders;

        Program() = default;

        Program(std::vector<ShaderMetadata>&& shaders):
            shaders(std::move(shaders))
        {}
    };
}