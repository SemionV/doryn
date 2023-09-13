#pragma once

namespace dory::openGL
{
    struct GlfwWindow: public dory::Window
    {
        GLFWwindow* handler;

        GlfwWindow(int id, GLFWwindow* handler):
            dory::Window(id),
            handler(handler)
        {}

        GlfwWindow(int id):
            dory::Window(id)
        {}
    };
}