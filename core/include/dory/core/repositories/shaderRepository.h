#pragma once

#include "dory/core/resources/entities/openglShader.h"
#include <dory/core/repositories/iShaderRepository.h>
#include <dory/core/repository.h>

namespace dory::core::repositories
{
    class ShaderRepository: public Repository<resources::entities::OpenglShader, resources::IdType, IShaderRepository>
    {};
}
