#pragma once

#include "../entity.h"

namespace dory::core::resources::bindings
{
    struct BufferBinding: public Entity<>
    {
        std::size_t size {};
    };
}