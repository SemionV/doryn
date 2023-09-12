#pragma once

#include "dependencies.h"

namespace doryOpenGL
{
    struct ShaderCompilationError
    {
        std::string compilationLog;

        ShaderCompilationError(std::string compilationLog):
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
}