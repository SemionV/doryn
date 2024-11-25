#pragma once

#include <dory/core/dependencyResolver.h>
#include <dory/core/services/graphics/iGpuDriver.h>

namespace dory::core::services::graphics
{
    class OpenglGpuDriver: public IGpuDriver, public DependencyResolver
    {
    private:
        bool checkForError();

    public:
        explicit OpenglGpuDriver(Registry& registry);

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
