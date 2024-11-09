#pragma once

#include <dory/core/resources/entities/shaderProgram.h>
#include "iRepository.h"

namespace dory::core::repositories
{
    class IShaderProgramRepository: public IRepository<resources::entities::ShaderProgram>
    {
    public:
        virtual resources::entities::ShaderProgram* get(std::string_view key, resources::IdType windowId) = 0;
    };
}

