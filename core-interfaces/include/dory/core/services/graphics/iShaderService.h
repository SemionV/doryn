#pragma once

#include "dory/generic/baseTypes.h"
#include <dory/core/resources/entities/shader.h>
#include <dory/core/resources/entities/shaderProgram.h>

namespace dory::core::services::graphics
{
    class IShaderService: public generic::Interface
    {
    public:
        virtual bool initializeProgram(resources::entities::ShaderProgram& program) = 0;
        virtual bool initializeShader(const resources::entities::ShaderProgram& program, resources::entities::Shader& shader, const std::string& sourceCode) = 0;
    };
}
