#pragma once

#include <dory/core/services/iWindowService.h>
#include <dory/core/resources/entities/window.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services
{
    class WindowService: public IWindowService
    {
    protected:
        Registry& _registry;

        resources::entities::Window& addWindow(resources::entities::Window& window);
        void removeWindow(resources::IdType windowId);
    public:
        explicit WindowService(Registry& registry);
    };
}

