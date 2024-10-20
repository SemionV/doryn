#pragma once

#include <string>
#include "iDevice.h"

namespace dory::core::devices
{
    class ITerminalDevice: public IDevice
    {
    public:
        virtual void write(const std::string& message) = 0;
        virtual void writeLine(const std::string& message) = 0;
        virtual void enterCommandMode() = 0;
        virtual void exitCommandMode() = 0;
        virtual bool isCommandMode() = 0;
    };
}
