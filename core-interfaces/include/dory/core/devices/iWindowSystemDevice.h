#pragma once

#include "iDevice.h"
#include <dory/core/resources/dataContext.h>

namespace dory::core::devices
{
    class IWindowSystemDevice: public IDevice
    {
    public:
        virtual void pollEvents(resources::DataContext& context) = 0;
    };
}
