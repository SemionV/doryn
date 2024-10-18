#pragma once

#include <string>

namespace dory::core::devices
{
    class IStandardOutputDevice
    {
    public:
        virtual ~IStandardOutputDevice() = default;

        virtual void out(const std::string& data) = 0;
        virtual void flush() = 0;
    };
}