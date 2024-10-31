#pragma once

#include <dory/core/resources/entity.h>
#include <GLFW/glfw3.h>

namespace dory::core::resources::entity
{
    class GlfwWindow: public entity::IWindowSubsystemData
    {
    public:
        explicit GlfwWindow(GLFWwindow* handler):
            handler(handler)
        {}

        GLFWwindow* handler{};
    };
}
