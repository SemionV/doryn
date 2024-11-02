#pragma once

#include <dory/core/services/iWindowService.h>

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

        resources::IdType initializeWindow(const resources::entity::Window& window);
        void removeWindow(resources::IdType windowId);
    public:
        explicit WindowService(Registry& registry);
    };
}

