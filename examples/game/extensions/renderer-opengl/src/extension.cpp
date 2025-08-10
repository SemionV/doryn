#ifndef DORY_OPENGL_INCLUDED
#include <glad/gl.h>
#define DORY_OPENGL_INCLUDED
#endif
#include <GLFW/glfw3.h>
#include <extension.h>
#include <shaderService.h>

namespace dory::renderer::opengl
{
    void Extension::attach(dory::generic::extension::LibraryHandle library, dory::core::resources::DataContext& dataContext)
    {
        _registry.get<core::services::ILogService, core::resources::Logger::App>([](core::services::ILogService* logger) {
            logger->information(std::string_view ("dory::renderer::opengl::Extension: attach extension"));
        });

        _registry.set<core::services::graphics::IShaderService, core::resources::GraphicalSystem::opengl>(library, _shaderService.get());
    }

    Extension::Extension(core::Registry& registry):
            _registry(registry),
            _shaderService(registry, std::make_shared<ShaderService>(registry), core::resources::GraphicalSystem::opengl)
    {}

    Extension::~Extension()
    {
        _registry.get<core::services::ILogService, core::resources::Logger::App>([](core::services::ILogService* logger) {
            logger->information(std::string_view ("dory::renderer::opengl::Extension: detach extension"));
        });
    }
}