#include <shaderProgramService.h>
#include <glad/gl.h>

namespace dory::renderer::opengl
{
    ShaderProgramService::ShaderProgramService(core::Registry& registry, ShaderRepository& shaderRepository, ProgramRepository& programRepository):
        _registry(registry),
        _shaderRepository(shaderRepository),
        _programRepository(programRepository)
    {}

    void ShaderProgramService::loadProgram(const resources::configuration::Program& programConfig)
    {
        auto program = resources::Program{programConfig.key};
        program.glId = glCreateProgram();

        auto id = _programRepository.insert(program);

        for(const auto& shaderConfig : programConfig.shaders)
        {

        }
    }
}