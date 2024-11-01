#pragma once

#include <dory/core/services/iWindowService.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services
{
    class GlfwWindowService: public IWindowService
    {
    private:
        Registry& _registry;

    public:
        explicit GlfwWindowService(Registry& registry);

        resources::IdType createWindow(const resources::WindowParameters& parameters) override;
        void closeWindow(resources::IdType windowId) override;
    };
}
