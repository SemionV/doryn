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
    private:
        bool initializeGraphics(const resources::entities::Window& window);

    public:
        explicit GlfwWindowService(Registry& registry);

        resources::IdType createWindow(const resources::WindowParameters& parameters) override;
        void closeWindow(resources::IdType windowId) override;
        void setCurrentWindow(const resources::entities::Window& window) override;
        void swapBuffers(const resources::entities::Window& window) override;
    };
}
