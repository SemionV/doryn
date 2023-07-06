#pragma once

#include "base/doryExport.h"
#include "base/messaging/message.h"

namespace dory
{
    struct DORY_API ConsoleMessage: public Message
    {
        public:
            char keyPressed;

        public:
            ConsoleMessage(char keyPressed);
    };
}