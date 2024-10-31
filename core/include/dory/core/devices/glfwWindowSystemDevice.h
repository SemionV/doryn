#pragma once

#include <dory/core/devices/iWindowSystemDevice.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::devices
{
    class GlfwWindowSystemDevice: public IWindowSystemDevice
    {
    private:
        Registry& _registry;

    public:
        explicit GlfwWindowSystemDevice(Registry& registry);

        void connect(resources::DataContext& context) override;
        void disconnect(resources::DataContext& context) override;
        void pollEvents(resources::DataContext& context) override;
    };
}
