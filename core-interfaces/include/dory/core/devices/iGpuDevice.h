#pragma once

#include "iDevice.h"
#include <dory/core/resources/bindings/meshBinding.h>
#include <dory/core/resources/bindings/bufferBinding.h>
#include <dory/core/resources/objects/frame.h>
#include <dory/core/resources/assets/shader.h>
#include <dory/core/resources/bindings/shaderBinding.h>
#include <dory/core/resources/profiling.h>
#include <dory/core/resources/assets/image.h>

namespace dory::core::devices
{
    class IGpuDevice: public IDevice
    {
    public:
        virtual bool allocateBuffer(resources::bindings::BufferBinding* bufferBinding, std::size_t size) = 0;
        virtual void deallocateBuffer(resources::bindings::BufferBinding* bufferBinding) = 0;
        virtual void writeData(resources::bindings::BufferBinding* bufferBinding, std::size_t offset, std::size_t size, const void* data) = 0;

        virtual void bindMesh(resources::bindings::MeshBinding* meshBinding,
                              const resources::bindings::BufferBinding* vertexBuffer,
                              const resources::bindings::BufferBinding* indexBuffer) = 0;
        virtual void bindShader(const std::string& sourceCode, resources::assets::ShaderType type, resources::bindings::ShaderBinding* shaderBinding) = 0;
        virtual void bindMaterial(resources::bindings::MaterialBinding* materialBinding, const std::vector<resources::bindings::ShaderBinding*>& shaders) = 0;

        virtual void drawFrame(const resources::objects::Frame& frame, resources::profiling::Profiling& profiling) = 0;
        virtual bool getFrontBufferImage(const resources::entities::View& view, resources::assets::Image& image) = 0;
        virtual bool getBackBufferImage(const resources::entities::View& view, resources::assets::Image& image) = 0;
    };
}