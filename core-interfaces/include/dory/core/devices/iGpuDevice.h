#pragma once

#include "iDevice.h"
#include <dory/core/resources/bindings/meshBinding.h>
#include <dory/core/resources/bindings/bufferBinding.h>

namespace dory::core::devices
{
    class IGpuDevice: public IDevice
    {
    public:
        virtual bool allocateBuffer(resources::bindings::BufferBinding* bufferBinding, std::size_t size) = 0;
        virtual void deallocateBuffer(resources::bindings::BufferBinding* bufferBinding) = 0;
        virtual void writeData(resources::bindings::BufferBinding* bufferBinding, std::size_t offset, std::size_t size, const void* data) = 0;
        virtual void setVertexAttributes(const resources::bindings::MeshBinding* meshBinding,
                                         const resources::bindings::BufferBinding* vertexBufferBinding,
                                         resources::bindings::VertexAttributeBinding* attributes,
                                         std::size_t count) = 0;
        virtual void initializeMesh(resources::bindings::MeshBinding* meshBinding) = 0;
    };
}
