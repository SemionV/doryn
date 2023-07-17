#pragma once

#include "base/doryExport.h"
#include "base/messaging/deviceMessage.h"

namespace dory
{
    class DORY_API ConsoleMessage: public DeviceMessage
    {
        public:
            char keyPressed;

        public:
            ConsoleMessage(char keyPressed);
    };
}