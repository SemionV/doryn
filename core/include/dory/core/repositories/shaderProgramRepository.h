#pragma once

#include "dory/core/resources/entities/openglShaderProgram.h"
#include <dory/core/repositories/iShaderProgramRepository.h>
#include <dory/core/repository.h>

namespace dory::core::repositories
{
    class ShaderProgramRepository: public Repository<resources::entities::OpenglShaderProgram, resources::IdType, IShaderProgramRepository>
    {
    public:
        resources::entities::ShaderProgram* get(std::string_view key, resources::IdType windowId) override;
    };
}
