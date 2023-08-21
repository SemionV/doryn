#pragma once

#include "base/doryExport.h"
#include "message.h"

namespace dory
{
    class DORY_API MessageFactory
    {
        public:
            std::shared_ptr<Message> createMessage(MessageType messageType)
            {
                Message* message = new Message(messageType);
                return std::shared_ptr<Message>(message);
            }

            template<class TMessage, typename ...T>
            std::enable_if_t<std::is_base_of_v<Message, TMessage>, std::shared_ptr<TMessage>> createMessage(T&& ...arguments) 
            {
                TMessage* message = new TMessage(std::forward<T>(arguments)...);
                return std::shared_ptr<TMessage>(message);
            }
    };
}