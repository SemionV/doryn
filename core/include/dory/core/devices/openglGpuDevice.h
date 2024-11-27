#pragma once

#include "dory/core/dependencyResolver.h"
#include "dory/core/devices/iGpuDevice.h"

namespace dory::core::devices
{
    class OpenglGpuDevice: public IGpuDevice, public DependencyResolver
    {
    private:
        bool checkForError();

    public:
        explicit OpenglGpuDevice(Registry& registry);

        void connect(resources::DataContext& context) override;
        void disconnect(resources::DataContext& context) override;

        bool allocateBuffer(resources::bindings::BufferBinding* bufferBinding, std::size_t size) override;
        void deallocateBuffer(resources::bindings::BufferBinding* bufferBinding) override;
        void writeData(resources::bindings::BufferBinding* bufferBinding, std::size_t offset, std::size_t size, const void* data) override;
        void bindMesh(resources::bindings::MeshBinding* meshBinding,
                      const resources::bindings::BufferBinding* vertexBuffer,
                      const resources::bindings::BufferBinding* indexBuffer) override;
        void bindShader(const std::string& sourceCode, resources::assets::ShaderType type, resources::bindings::ShaderBinding* shaderBinding) override;
        void bindMaterial(resources::bindings::MaterialBinding* materialBinding, const std::vector<resources::bindings::ShaderBinding*>& shaders) override;
        void drawFrame(const resources::objects::Frame& frame) override;
    };
}
