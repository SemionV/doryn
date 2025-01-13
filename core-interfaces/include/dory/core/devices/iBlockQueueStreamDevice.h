#pragma once

#include "iDevice.h"

namespace dory::core::devices
{
    class IImageStreamDevice: public IDevice
    {
    public:
        virtual void updateStream() = 0;
    };
}