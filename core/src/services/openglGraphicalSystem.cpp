#include <dory/core/services/openglGraphicalSystem.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <dory/core/resources/window.h>
#include <dory/core/resources/graphicalSystem.h>

namespace dory::core::services
{
    bool OpenglGraphicalSystem::initializeGraphics(const resources::entity::Window& window)
    {
        if(window.graphicalSystem == resources::GraphicalSystem::opengl)
        {
            if(window.windowSystem == resources::WindowSystem::glfw)
            {
                auto specificData = std::static_pointer_cast<resources::entity::GlfwWindow>(window.windowSystemData);

                glfwMakeContextCurrent(specificData->handler);
                int version = gladLoadGL(glfwGetProcAddress);
                return version != 0;
            }
        }

        return false;
    }
}