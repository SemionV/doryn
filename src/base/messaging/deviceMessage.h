#pragma once

#include "base/doryExport.h"
#include "message.h"

namespace dory
{
    struct DORY_API DeviceMessage: public Message
    {
        protected:
            DeviceMessage(MessageType messageType);
    };
}