#pragma once

#include <dory/core/dependencyResolver.h>
#include <dory/core/devices/iDisplaySystemDevice.h>

namespace dory::core::devices
{
    class GlfwDisplaySystemDevice: public IDisplaySystemDevice, public DependencyResolver
    {
    private:
        void updateDisplays() const;

    public:
        explicit GlfwDisplaySystemDevice(Registry& registry);

        void connect(resources::DataContext& context) final;
        void disconnect(resources::DataContext& context) final;
    };
}