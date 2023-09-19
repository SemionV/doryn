#pragma once

namespace dory::openGL
{
    struct GlfwWindow: public domain::entity::Window
    {
        GLFWwindow* handler;

        GlfwWindow(dory::domain::entity::IdType id, GLFWwindow* handler):
            domain::entity::Window(id),
            handler(handler)
        {}
    };
}