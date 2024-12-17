#pragma once

#include "iDevice.h"
#include <dory/core/resources/dataContext.h>
#include <dory/core/resources/entities/window.h>

namespace dory::core::devices
{
    class IWindowSystemDevice: public IDevice
    {
    public:
        virtual void pollEvents(resources::DataContext& context) = 0;
        virtual void setupWindow(resources::entities::Window& window) = 0;
    };
}
