#pragma once

#include "base/doryExport.h"
#include "base/messaging/message.h"
#include "base/messaging/messages/consoleMessage.h"

namespace dory
{
    class DORY_API MessagePool
    {
        private: 
            std::vector<ConsoleMessage> messages;
        
        public:
            MessagePool();
            void addMessage(ConsoleMessage& message);
            void clean();
            void iterate(std::function<void(ConsoleMessage)> lambda);
    };
}