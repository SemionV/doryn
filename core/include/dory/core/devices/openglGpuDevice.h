#pragma once

#include "dory/core/dependencyResolver.h"
#include "dory/core/devices/iGpuDevice.h"

namespace dory::core::devices
{
    class OpenglGpuDriver: public IGpuDevice, public DependencyResolver
    {
    private:
        bool checkForError();

    public:
        explicit OpenglGpuDriver(Registry& registry);

        void connect(resources::DataContext& context) override;
        void disconnect(resources::DataContext& context) override;

        bool allocateBuffer(resources::bindings::BufferBinding* bufferBinding, std::size_t size) override;
        void deallocateBuffer(resources::bindings::BufferBinding* bufferBinding) override;
        void writeData(resources::bindings::BufferBinding* bufferBinding, std::size_t offset, std::size_t size, const void* data) override;
        void setVertexAttributes(const resources::bindings::MeshBinding* meshBinding,
                                 const resources::bindings::BufferBinding* vertexBufferBinding,
                                 resources::bindings::VertexAttributeBinding* attributes,
                                 std::size_t count) override;
        void initializeMesh(resources::bindings::MeshBinding* meshBinding) override;
    };
}
