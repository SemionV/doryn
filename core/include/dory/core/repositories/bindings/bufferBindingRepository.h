#pragma once

#include <dory/core/repositories/bindings/iBufferBindingRepository.h>
#include "dory/core/resources/bindings/openglBufferBinding.h"

namespace dory::core::repositories::bindings
{
    class BufferBindingRepository final : public Repository<resources::bindings::OpenglBufferBinding, resources::IdType, IBufferBindingRepository>
    {};
}