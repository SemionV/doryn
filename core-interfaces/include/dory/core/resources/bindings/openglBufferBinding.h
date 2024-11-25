#pragma once

#include "bufferBinding.h"

namespace dory::core::resources::bindings
{
    struct OpenglBufferBinding: public BufferBinding
    {
        GLuint glId {};
    };
}
