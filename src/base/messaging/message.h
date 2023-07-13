#pragma once

#include "base/doryExport.h"
#include "messageType.h"

namespace dory
{
    class DORY_API Message
    {
        public:
            const MessageType messageType;

        public:
            Message(MessageType messageType);
    };
}