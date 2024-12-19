#pragma once

#include <dory/core/services/iWindowService.h>
#include <dory/core/dependencyResolver.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services
{
    class WindowService: public IWindowService, public DependencyResolver
    {
    public:
        explicit WindowService(Registry& registry);

        resources::entities::Window* createWindow(const resources::WindowParameters& parameters, resources::WindowSystem windowSystem) override;
        void closeWindow(resources::IdType windowId, resources::WindowSystem windowSystem) override;
        void setCurrentWindow(const resources::entities::Window& window) override;
        void swapBuffers(const resources::entities::Window& window) override;
    };
}
