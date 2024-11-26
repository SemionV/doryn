#pragma once

#include <dory/core/repositories/assets/iShaderRepository.h>

namespace dory::core::repositories::assets
{
    class ShaderRepository: public Repository<resources::assets::Shader, resources::IdType, IShaderRepository>
    {};
}