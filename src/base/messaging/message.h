#pragma once

#include "base/doryExport.h"
#include "messageType.h"

namespace dory
{
    class DORY_API MessageItem
    {
        public:
            const int messageType;
            const std::shared_ptr<void> data;

        public:
            MessageItem(int messageType);
            MessageItem(int messageType, std::shared_ptr<void> data);
    };

    class DORY_API Message
    {
        public:
            friend class MessageFactory;

            const MessageType messageType;

        protected:
            Message(MessageType messageType);
    };

    template<typename TData>
    class DORY_API DataMessage final: public Message
    {
        public:
            friend class MessageFactory;

            const TData data;

        protected:
            DataMessage(MessageType messageType, TData&& data):
                Message(messageType),
                data(std::forward<TData>(data))
            {

            }
    };
}