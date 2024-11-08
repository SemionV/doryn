#include <dory/core/registry.h>
#include <shaderProgramService.h>
#include <glad/gl.h>
#include <cstring>

namespace dory::renderer::opengl
{
    ShaderProgramService::ShaderProgramService(core::Registry& registry, ShaderRepository& shaderRepository, ProgramRepository& programRepository):
        _registry(registry),
        _shaderRepository(shaderRepository),
        _programRepository(programRepository)
    {}

    bool ShaderProgramService::loadProgram(const core::resources::configuration::Program& programConfig)
    {
        auto program = resources::Program{programConfig.key};
        _programRepository.insert(program);

        bool result {};

        for(const auto& shaderConfig : programConfig.shaders)
        {
            result = loadShader(shaderConfig, program) && result;
        }

        return result;
    }

    bool ShaderProgramService::loadShader(const core::resources::configuration::Shader& shaderConfig, const resources::Program& program)
    {
        bool result;

        _registry.get<
                generic::registry::Service<core::services::IFileService>,
                generic::registry::Service<core::services::ILogService>>(
            [this, &result, &program, &shaderConfig](core::services::IFileService* fileService, core::services::ILogService* logger)
        {
            try
            {
                auto shader = resources::Shader{};
                shader.programId = program.id;
                if(shaderConfig.type == resources::ShaderType::vertexShader)
                {
                    shader.type = GL_VERTEX_SHADER;
                }
                else if(shaderConfig.type == resources::ShaderType::fragmentShader)
                {
                    shader.type = GL_FRAGMENT_SHADER;
                }
                _shaderRepository.insert(shader);

                shader.filePath = shaderConfig.filename;
                shader.sourceCode = fileService->read(shader.filePath);

                if(!shader.sourceCode.empty())
                {
                    const char* shaderSource = shader.sourceCode.c_str();
                    glShaderSource(shader.id, 1, &shaderSource, 0);

                    glCompileShader(shader.id);

                    GLint compiled;
                    glGetShaderiv(shader.id, GL_COMPILE_STATUS, &compiled);
                    if(!compiled)
                    {
                        GLsizei len;
                        glGetShaderiv(shader.id, GL_INFO_LOG_LENGTH, &len);

                        std::string logString(len + 1, 0);
                        glGetShaderInfoLog(shader.id, len, &len, logString.data());

                        throw std::logic_error{logString};
                    }

                    glAttachShader(program.id, shader.id);

                    logger->information(std::string{"shader loaded: "} + shader.filePath.string());

                    result = true;
                }
            }
            catch(const std::exception& e)
            {
                std::string errorMessage = std::strlen(e.what()) ? std::string{",\n"} + e.what() : "";
                logger->error("cannot load shader: " + shaderConfig.filename + errorMessage);
            }

            result = false;
        });

        return result;
    }
}