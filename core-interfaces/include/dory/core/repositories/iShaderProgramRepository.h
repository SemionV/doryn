#pragma once

#include <dory/core/resources/entities/shaderProgram.h>
#include "iRepository.h"

namespace dory::core::repositories
{
    class IShaderProgramRepository: public IRepository<resources::entities::ShaderProgram>
    {};
}

