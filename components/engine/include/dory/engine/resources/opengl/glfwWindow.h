#pragma once

namespace dory::engine::resources::opengl
{
    struct GlfwWindow: public resources::entity::Window
    {
        GLFWwindow* handler{};

        explicit GlfwWindow(resources::entity::IdType id, GLFWwindow* handler):
            resources::entity::Window(id),
            handler(handler)
        {}
    };
}