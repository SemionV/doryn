#pragma once

#include "iDevice.h"
#include <dory/core/resources/id.h>

namespace dory::core::devices
{
    class IFileWatcherDevice: public IDevice
    {
    public:
         virtual void updateWatches() = 0;
    };
}