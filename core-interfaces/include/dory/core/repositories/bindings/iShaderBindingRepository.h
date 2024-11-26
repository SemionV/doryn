#pragma once

#include "dory/core/resources/bindings/shaderBinding.h"

namespace dory::core::repositories::bindings
{
    class IShaderBindingRepository: public IRepository<resources::bindings::ShaderBinding>
    {};
}