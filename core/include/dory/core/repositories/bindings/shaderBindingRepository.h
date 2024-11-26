#pragma once

#include <dory/core/repositories/bindings/iShaderBindingRepository.h>
#include "dory/core/resources/bindings/openglShaderBinding.h"

namespace dory::core::repositories::bindings
{
    class ShaderBindingRepository: public Repository<resources::bindings::OpenglShaderBinding, resources::IdType, IShaderBindingRepository>
    {};
}