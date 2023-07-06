#pragma once

#include "base/doryExport.h"
#include "base/messaging/message.h"

namespace dory
{
    class DORY_API ConsoleMessage: public Message
    {
        public:
            char keyPressed;
            int clickX;
            int clickY;

        public:
            ConsoleMessage(char keyPressed, int clickX, int clickY);
    };
}