#pragma once

#include "dory/core/resources/assets/shader.h"

namespace dory::core::repositories::assets
{
    class IShaderRepository: public IRepository<resources::assets::Shader>
    {};
}