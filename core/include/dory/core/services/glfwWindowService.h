#pragma once

#include <dory/core/services/windowService.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services
{
    class GlfwWindowService: public WindowService
    {
    public:
        explicit GlfwWindowService(Registry& registry);

        resources::entities::Window& createWindow(const resources::WindowParameters& parameters) override;
        void closeWindow(resources::IdType windowId) override;
        void setCurrentWindow(resources::IdType windowId) override;
        void swapBuffers(const resources::entities::Window& window) override;
    };
}
