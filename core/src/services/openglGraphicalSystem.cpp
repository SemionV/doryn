#include <dory/core/registry.h>
#include <dory/core/services/openglGraphicalSystem.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <dory/core/resources/glfwWindow.h>
#include <dory/core/resources/graphicalSystem.h>
#include <dory/core/services/iRenderer.h>

namespace dory::core::services
{
    OpenglGraphicalSystem::OpenglGraphicalSystem(Registry& registry):
        _registry(registry)
    {}

    bool OpenglGraphicalSystem::initializeGraphics(const resources::entities::Window& window)
    {
        if(window.windowSystem == resources::WindowSystem::glfw)
        {
            auto& glfwWindow = (resources::entities::GlfwWindow&)window;

            glfwMakeContextCurrent(glfwWindow.handler);
            int version = gladLoadGL(glfwGetProcAddress);
            return version != 0;
        }

        return false;
    }

    void OpenglGraphicalSystem::setCurrentWindow(const resources::entities::Window& window)
    {
        if(window.windowSystem == resources::WindowSystem::glfw)
        {
            auto& glfwWindow = (resources::entities::GlfwWindow&)window;
            glfwMakeContextCurrent(glfwWindow.handler);
        }
    }

    void OpenglGraphicalSystem::swapBuffers(const resources::entities::Window& window)
    {
        if(window.windowSystem == resources::WindowSystem::glfw)
        {
            auto& glfwWindow = (resources::entities::GlfwWindow&)window;
            glfwSwapBuffers(glfwWindow.handler);
        }
    }
}