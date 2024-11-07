#pragma once

#include <dory/core/resources/entities/shader.h>
#include "iRepository.h"

namespace dory::core::repositories
{
    class IShaderRepository: public IRepository<resources::entities::Shader>
    {};
}

