#pragma once

#include <string>
#include "iDevice.h"

namespace dory::core::devices
{
    class IStandardIODevice: public IDevice
    {
    public:
        ~IStandardIODevice() override = default;

        virtual void out(const std::string& data) = 0;
        virtual void flush() = 0;
    };
}