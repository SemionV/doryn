#pragma once

#include <dory/engine/domain/configuration.h>
#include <dory/engine/domain/services/logService.h>
#include "../graphics/program.h"

namespace dory::opengl::services
{
    struct ShaderCompilationError
    {
        std::string compilationLog;
        std::string shaderIdentifier;

        ShaderCompilationError(std::string shaderIdentifier, std::string compilationLog):
            shaderIdentifier(shaderIdentifier),
            compilationLog(compilationLog)
        {};
    };

    struct ShaderProgramLinkingError
    {
        std::string linkingLog;

        ShaderProgramLinkingError(std::string linkingLog):
            linkingLog(linkingLog)
        {};
    };

    struct ShaderServiceError
    {
        std::shared_ptr<ShaderCompilationError> shaderCompilationError;
        std::shared_ptr<ShaderProgramLinkingError> shaderProgramLinkingError;
    };

    template<typename TImplementation>
    class IShaderService: NonCopyable, public StaticInterface<TImplementation>
    {
    public:
        void loadProgram(graphics::Program& program, const std::function<void(ShaderServiceError&)>& errorHandler = nullptr)
        {
            this->toImplementation()->loadProgramImpl(program, errorHandler);
        }
    };

    template<typename TLogger, typename TFileService>
    class ShaderService: public IShaderService<ShaderService<TLogger, TFileService>>
    {
    private:
        using LoggerType = domain::services::ILogService<TLogger>;
        LoggerType& logger;

        using FileServiceType = domain::services::IFileService<TFileService>;
        FileServiceType& fileService;

        const configuration::ShaderLoader& shaderLoaderSettings;

    public:
        explicit ShaderService(const configuration::ShaderLoader& shaderLoaderSettings,
                               LoggerType& logger,
                               FileServiceType& fileService):
            shaderLoaderSettings(shaderLoaderSettings),
            logger(logger),
            fileService(fileService)
        {}

        void loadProgramImpl(graphics::Program& program, const std::function<void(ShaderServiceError&)>& errorHandler)
        {
            program.id = glCreateProgram();

            auto& shaders = program.shaders;
            std::size_t shadersCount = shaders.size();
            for(std::size_t i = 0; i < shadersCount; ++i)
            {
                auto& shader = shaders[i];

                auto filename = std::filesystem::path{shaderLoaderSettings.shadersDirectory} /= shader.key;
                try
                {
                    shader.sourceCode = fileService.read(filename);
                }
                catch(const std::exception& e)
                {
                    logger.error("cannot load shader: " + filename.string());
                }

                if(!shader.sourceCode.empty())
                {
                    auto shaderId = shader.id = glCreateShader(shader.type);
                    const char* shaderSource = shader.sourceCode.c_str();
                    glShaderSource(shaderId, 1, &shaderSource, 0);

                    glCompileShader(shaderId);

                    GLint compiled;
                    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled);
                    if(!compiled)
                    {
                        GLsizei len;
                        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &len);

                        std::string logString(len + 1, 0);
                        glGetShaderInfoLog(shaderId, len, &len, logString.data());

                        if(errorHandler)
                        {
                            ShaderServiceError shaderServiceError;
                            shaderServiceError.shaderCompilationError = std::make_shared<ShaderCompilationError>(shader.key, logString);

                            errorHandler(shaderServiceError);
                        }
                    }

                    glAttachShader(program.id, shaderId);
                }
            }

            glLinkProgram(program.id);

            GLint linked;
            glGetProgramiv(program.id, GL_LINK_STATUS, &linked);
            if (!linked)
            {
                for(std::size_t i = 0; i < shadersCount; ++i)
                {
                    auto& shader = shaders[i];
                    glDeleteShader(shader.id);
                    shader.id = 0;
                }

                GLsizei len;
                glGetProgramiv(program.id, GL_INFO_LOG_LENGTH, &len);

                std::string logString(len + 1, 0);
                glGetProgramInfoLog(program.id, len, &len, logString.data());

                if(errorHandler)
                {
                    ShaderServiceError shaderServiceError;
                    shaderServiceError.shaderProgramLinkingError = std::make_shared<ShaderProgramLinkingError>(logString);

                    errorHandler(shaderServiceError);
                }
            }
        }
    };
}