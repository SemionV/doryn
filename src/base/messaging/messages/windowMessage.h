#pragma once

#include "base/messaging/deviceMessage.h"
#include "base/devices/window.h"

namespace dory
{
    class DORY_API WindowMessage: public DeviceMessage
    {
        public:
            const std::shared_ptr<Window> window;

            int x;
            int y;

        public:
            WindowMessage(MessageType messageType, std::shared_ptr<Window> window);
    };
}