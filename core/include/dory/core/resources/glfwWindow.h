#pragma once

#include <dory/core/resources/entity.h>
#include <dory/core/resources/entities/window.h>

namespace dory::core::resources::entities
{
    class GlfwWindow: public entities::Window
    {
    public:
        GlfwWindow() = default;

        explicit GlfwWindow(IdType id, WindowSystem windowSystem, IdType graphicalContextId):
                Window(id, windowSystem, graphicalContextId)
        {}

        explicit GlfwWindow(const entities::Window& window):
                entities::Window(window)
        {}

        GLFWwindow* handler{};
    };
}
