#include <dory/core/registry.h>
#include <shaderService.h>
#ifndef DORY_OPENGL_INCLUDED
#include <glad/gl.h>
#define DORY_OPENGL_INCLUDED
#endif
#include <dory/core/resources/entities/openglShader.h>
#include <dory/core/resources/entities/openglShaderProgram.h>

namespace dory::renderer::opengl
{
    ShaderService::ShaderService(core::Registry &registry):
        _registry(registry)
    {}

    bool ShaderService::initializeProgram(core::resources::entities::ShaderProgram& program)
    {
        auto& openglProgram = (core::resources::entities::OpenglShaderProgram&)program;
        if(openglProgram.index != GL_INVALID_INDEX)
        {
            glDeleteProgram(openglProgram.index);
        }

        openglProgram.index = glCreateProgram();

        return openglProgram.index != GL_INVALID_INDEX;
    }

    bool ShaderService::initializeShader(const core::resources::entities::ShaderProgram& program,
                                         core::resources::entities::Shader& shader, const std::string& sourceCode)
    {
        auto& openglProgram = (core::resources::entities::OpenglShaderProgram&)program;
        auto& openglShader = (core::resources::entities::OpenglShader&)shader;

        if(openglShader.index != GL_INVALID_INDEX)
        {
            glDeleteShader(openglShader.index);
        }

        if(openglShader.type == core::resources::entities::ShaderType::glVertexShader)
        {
            openglShader.glType = GL_VERTEX_SHADER;
        }
        else if(openglShader.type == core::resources::entities::ShaderType::glFragmentShader)
        {
            openglShader.glType = GL_FRAGMENT_SHADER;
        }

        openglShader.index = glCreateShader(openglShader.glType);

        const char* shaderSource = sourceCode.c_str();
        glShaderSource(openglShader.index, 1, &shaderSource, 0);

        GLint compiled;
        glGetShaderiv(openglShader.index, GL_COMPILE_STATUS, &compiled);
        if(!compiled)
        {
            GLsizei len;
            glGetShaderiv(openglShader.index, GL_INFO_LOG_LENGTH, &len);

            std::string logString(len + 1, 0);
            glGetShaderInfoLog(openglShader.index, len, &len, logString.data());

            _registry.get<core::services::ILogService>([&logString, &shader](core::services::ILogService* logger) {
                logger->error("Shader compilation error: " + shader.filePath.string() + "\n" + logString);
            });

            return false;
        }

        glAttachShader(openglProgram.index, openglShader.index);

        return openglShader.index != GL_INVALID_INDEX;
    }
}