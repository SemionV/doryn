#pragma once

#include "shaderRepository.h"
#include "programRepository.h"
#include "resources/configuration.h"

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

        void loadProgram(const resources::configuration::Program& programConfig);
    };
}
