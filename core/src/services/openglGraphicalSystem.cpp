#include <dory/core/registry.h>
#include <dory/core/services/openglGraphicalSystem.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <dory/core/resources/window.h>
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
            auto specificData = std::static_pointer_cast<resources::entity::GlfwWindow>(window.windowSystemData);

            glfwMakeContextCurrent(specificData->handler);
            int version = gladLoadGL(glfwGetProcAddress);
            return version != 0;
        }

        return false;
    }

    void OpenglGraphicalSystem::setCurrentWindow(const resources::entities::Window& window)
    {
        if(window.windowSystem == resources::WindowSystem::glfw)
        {
            auto specificData = std::static_pointer_cast<resources::entity::GlfwWindow>(window.windowSystemData);
            glfwMakeContextCurrent(specificData->handler);
        }
    }

    void OpenglGraphicalSystem::swapBuffers(const resources::entities::Window& window)
    {
        if(window.windowSystem == resources::WindowSystem::glfw)
        {
            auto specificData = std::static_pointer_cast<resources::entity::GlfwWindow>(window.windowSystemData);
            glfwSwapBuffers(specificData->handler);
        }
    }
}