#pragma once

#include "iDevice.h"

namespace dory::core::devices
{
    class IBlockQueueStreamDevice: public IDevice
    {
    public:
        virtual void updateStream() = 0;
    };
}