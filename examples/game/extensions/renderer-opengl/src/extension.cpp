#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <extension.h>
#include <openglRenderer.h>
#include <shaderService.h>

namespace dory::renderer::opengl
{
    void Extension::attach(dory::generic::extension::LibraryHandle library, dory::core::resources::DataContext& dataContext)
    {
        _registry.get<core::services::ILogService, core::resources::Logger::App>([](core::services::ILogService* logger) {
            logger->information(std::string_view ("dory::renderer::opengl::Extension: attach extension"));
        });

        _registry.set<core::services::graphics::IRenderer, core::resources::GraphicalSystem::opengl>(library, _renderer.get());
        _registry.set<core::services::graphics::IShaderService, core::resources::GraphicalSystem::opengl>(library, _shaderService.get());
    }

    Extension::Extension(core::Registry& registry):
            _registry(registry),
            _renderer(registry, std::make_shared<OpenglRenderer>(registry), core::resources::GraphicalSystem::opengl),
            _shaderService(registry, std::make_shared<ShaderService>(registry), core::resources::GraphicalSystem::opengl)
    {
#ifdef DORY_PLATFORM_WIN32
        glfwInit();
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        GLFWwindow* hidden_window = glfwCreateWindow(1, 1, "", NULL, NULL);
        glfwMakeContextCurrent(hidden_window);
        int version = gladLoadGL(glfwGetProcAddress);
        if (version == 0)
            {
            auto logger = _registry.get<core::services::ILogService>();
            if(logger)
            {
                logger->error(std::string_view("Failed to initialize OpenGL context for extension"));
            }
        }
        glfwDestroyWindow(hidden_window);
#endif
    }

    Extension::~Extension()
    {
        _registry.get<core::services::ILogService, core::resources::Logger::App>([](core::services::ILogService* logger) {
            logger->information(std::string_view ("dory::renderer::opengl::Extension: detach extension"));
        });
    }
}