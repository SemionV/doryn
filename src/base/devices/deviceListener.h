#pragma once

#include "base/doryExport.h"
#include "base/messaging/messagePool.h"
#include "deviceMessage.h"

namespace dory
{
    class DORY_API DeviceListener
    {
        public:
            virtual void handleMessage(std::shared_ptr<DeviceMessage> message) = 0;
            virtual void readUpdates(MessagePool& messagePool) = 0;

            virtual bool connect() = 0;
            virtual void disconnect() = 0;
    };
}