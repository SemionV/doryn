#pragma once

#include "base/doryExport.h"
#include "base/messaging/messagePool.h"

namespace dory
{
    class DORY_API Device
    {
        public:
            virtual bool connect() = 0;
            virtual void disconnect() = 0;
            virtual void readUpdates(MessagePool& messagePool) = 0;
    };
}