#pragma once

#include <glad/gl.h>
#include "dory/core/dependencyResolver.h"
#include "dory/core/devices/iGpuDevice.h"
#include "dory/core/resources/bindings/openglMaterialBinding.h"
#include "dory/core/resources/bindings/uniforms.h"

namespace dory::core::devices
{
    struct OpenglProperties
    {
        GLint bufferAlignment {};
    };

    class OpenglGpuDevice: public IGpuDevice, public DependencyResolver
    {
    private:
        OpenglProperties openglProperties;

        bool checkForError(const std::string& location);
        template<typename TUniform>
        void bindUniformLocations(resources::bindings::OpenglMaterialBinding* materialBinding, resources::bindings::UniformBinding& uniforms);
        void bindUniforms(resources::bindings::OpenglMaterialBinding* materialBinding);
        void setActiveMaterial(const resources::bindings::uniforms::DynamicUniforms& uniforms, const resources::bindings::MaterialBinding* materialBinding);
        static void fillUniforms(resources::bindings::uniforms::DynamicUniforms& uniforms, const resources::bindings::MaterialBinding* materialBinding);
        static void fillUniforms(resources::bindings::uniforms::StaticUniforms& uniforms, const resources::bindings::MaterialBinding* materialBinding);
        static void drawMesh(const resources::bindings::MeshBinding* meshBinding);

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
