#pragma once

#include "dependencies.h"
#include "olgDependencies.h"
#include "shaderServiceError.h"
#include "shaderMetadata.h"

namespace doryOpenGL
{
    class ShaderService
    {
        public:
            static GLuint buildProgram(const std::vector<ShaderMetadata>& shadersMetadata, std::function<bool(ShaderServiceError)> errorHandler = 0)
            {
                GLuint programId = glCreateProgram();

                std::size_t shadersCount = shadersMetadata.size();
                for(std::size_t i = 0; i < shadersCount; ++i)
                {
                    auto shaderMetadata = shadersMetadata[i];

                    auto shaderId = shaderMetadata.shaderId = glCreateShader(shaderMetadata.type);
                    const char* shaderSource = shaderMetadata.shaderSource.c_str();
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
                            shaderServiceError.shaderCompilationError = std::make_shared<ShaderCompilationError>(logString);

                            if(errorHandler(shaderServiceError))
                            {
                                return programId;
                            }
                        }
                        else
                        {
                            return programId;
                        }
                    }

                    glAttachShader(programId, shaderId);
                }

                glLinkProgram(programId);

                GLint linked;
                glGetProgramiv(programId, GL_LINK_STATUS, &linked);
                if (!linked)
                {
                    for(std::size_t i = 0; i < shadersCount; ++i) 
                    {
                        auto shaderMetadata = shadersMetadata[i];
                        glDeleteShader(shaderMetadata.shaderId);
                        shaderMetadata.shaderId = 0;
                    }

                    GLsizei len;
                    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &len);

                    std::string logString(len + 1, 0);
                    glGetProgramInfoLog(programId, len, &len, logString.data());

                    if(errorHandler)
                    {
                        ShaderServiceError shaderServiceError;
                        shaderServiceError.shaderProgramLinkingError = std::make_shared<ShaderProgramLinkingError>(logString);

                        if(errorHandler(shaderServiceError))
                        {
                            return programId;
                        }
                    }
                }

                return programId;
            }
    };
}