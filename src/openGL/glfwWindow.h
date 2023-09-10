#pragma once

namespace doryOpenGL
{
    struct GlfwWindow: public dory::Window
    {
        GLFWwindow* handler;
    };
}