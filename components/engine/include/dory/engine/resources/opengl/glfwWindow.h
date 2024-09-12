#pragma once

namespace dory::opengl
{
    struct GlfwWindow: public domain::entity::Window
    {
        GLFWwindow* handler;

        explicit GlfwWindow(dory::domain::entity::IdType id, GLFWwindow* handler):
            domain::entity::Window(id),
            handler(handler)
        {}
    };
}