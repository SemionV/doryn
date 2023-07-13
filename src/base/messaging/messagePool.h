#pragma once

#include "base/doryExport.h"
#include "base/messaging/message.h"
#include "base/messaging/messages/consoleMessage.h"

namespace dory
{
    class DORY_API MessagePool
    {
        private: 
            std::vector<std::shared_ptr<Message>> messages;
        
        public:
            MessagePool();
            void addMessage(std::shared_ptr<Message> message);
            void clean();
            void iterate(std::function<void(std::shared_ptr<Message>)> lambda);
    };
}