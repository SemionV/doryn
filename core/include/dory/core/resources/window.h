#pragma once

#include <dory/core/resources/entity.h>
#include <GLFW/glfw3.h>
#include <dory/core/resources/entities/window.h>

namespace dory::core::resources::entity
{
    class GlfwWindow: public entities::IWindowSubsystemData
    {
    public:
        explicit GlfwWindow(GLFWwindow* handler):
            handler(handler)
        {}

        GLFWwindow* handler{};
    };

    class GlfwWindow2: public entities::Window
    {
        GlfwWindow2() = default;

        explicit GlfwWindow2(IdType id, WindowSystem windowSystem, GraphicalSystem graphicalSystem):
                Window(id, windowSystem, graphicalSystem)
        {}

        GLFWwindow* handler{};
    };
}
