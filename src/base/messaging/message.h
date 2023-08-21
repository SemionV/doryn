#pragma once

#include "base/doryExport.h"
#include "messageType.h"

namespace dory
{
    class DORY_API Message
    {
        public:
            friend class MessageFactory;

            const MessageType messageType;

        protected:
            Message(MessageType messageType);
    };
}