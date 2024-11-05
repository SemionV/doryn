#pragma once

#include "shaderRepository.h"
#include "programRepository.h"
#include <dory/core/resources/configuration.h>

namespace dory::core
{
    class Registry;
}

namespace dory::renderer::opengl
{
    class ShaderProgramService
    {
    private:
        ShaderRepository& _shaderRepository;
        ProgramRepository& _programRepository;
        core::Registry& _registry;

    public:
        explicit ShaderProgramService(core::Registry& registry, ShaderRepository& shaderRepository, ProgramRepository& programRepository);

        bool loadProgram(const core::resources::configuration::Program& programConfig);
        bool loadShader(const core::resources::configuration::Shader& shaderConfig, const resources::Program& program);
    };
}
