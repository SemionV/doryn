#pragma once

namespace dory::openGL
{
    struct GlfwWindow: public dory::Window
    {
        GLFWwindow* handler;

        GlfwWindow(GLFWwindow* handler):
            handler(handler)
        {}
    };
}