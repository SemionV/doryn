#pragma once

#include "dory/core/resources/bindings/bufferBinding.h"

namespace dory::core::repositories::bindings
{
    class IBufferBindingRepository: public IRepository<resources::bindings::BufferBinding>
    {};
}
