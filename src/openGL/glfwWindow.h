#pragma once

namespace dory::openGL
{
    struct GlfwWindow: public domain::entity::Window
    {
        GLFWwindow* handler;

        GlfwWindow(GLFWwindow* handler):
            handler(handler)
        {}
    };
}