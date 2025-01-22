#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "dory/core/devices/iGpuDevice.h"
#include <dory/core/resources/dataContext.h>

class GpuDeviceMock: public dory::core::devices::IGpuDevice
{
public:
    MOCK_METHOD(void, connect, (dory::core::resources::DataContext& context));
    MOCK_METHOD(void, disconnect, (dory::core::resources::DataContext& context));
    MOCK_METHOD(bool, allocateBuffer, (dory::core::resources::bindings::BufferBinding* bufferBinding, std::size_t size));
    MOCK_METHOD(void, deallocateBuffer, (dory::core::resources::bindings::BufferBinding* bufferBinding));
    MOCK_METHOD(void, writeData, (dory::core::resources::bindings::BufferBinding* bufferBinding, std::size_t offset, std::size_t size, const void* data));
    MOCK_METHOD(void, bindMesh, (dory::core::resources::bindings::MeshBinding* meshBinding, const dory::core::resources::bindings::BufferBinding* vertexBuffer, const dory::core::resources::bindings::BufferBinding* indexBuffer));
    MOCK_METHOD(void, bindShader, (const std::string& sourceCode, dory::core::resources::assets::ShaderType type, dory::core::resources::bindings::ShaderBinding* shaderBinding));
    MOCK_METHOD(void, bindMaterial, (dory::core::resources::bindings::MaterialBinding* materialBinding, const std::vector<dory::core::resources::bindings::ShaderBinding*>& shaders));
    MOCK_METHOD(void, drawFrame, (const dory::core::resources::objects::Frame& frame, dory::core::resources::profiling::Profiling& profiling));
    MOCK_METHOD(void, completeFrame, (const dory::core::resources::objects::Frame& frame, dory::core::resources::profiling::Profiling& profiling));
    MOCK_METHOD(bool, getFrontBufferImage, (const dory::core::resources::entities::View& view, dory::core::resources::assets::Image& image));
    MOCK_METHOD(bool, getBackBufferImage, (const dory::core::resources::entities::View& view, dory::core::resources::assets::Image& image));
};

