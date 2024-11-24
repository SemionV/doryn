#pragma once

#include <dory/generic/baseTypes.h>

namespace dory::core::services::graphics
{
    class IGpuDriver: public generic::Interface
    {
    public:
        virtual bool allocateBuffer(resources::bindings::BufferBinding* bufferBinding, std::size_t size) = 0;
        virtual void deallocateBuffer(resources::bindings::BufferBinding* bufferBinding) = 0;
        virtual void writeData(resources::bindings::BufferBinding* bufferBinding, std::size_t offset, std::size_t size, const void* data) = 0;
    };
}
