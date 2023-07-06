#pragma once

#include "base/doryExport.h"
#include "base/messaging/message.h"

namespace dory
{
    class DORY_API MouseMessage: public Message
    {
        public:
            char buttonPressed;

        public:
            MouseMessage(char buttonPressed);
    };
}