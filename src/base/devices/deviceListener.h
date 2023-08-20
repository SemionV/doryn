#pragma once

#include "base/doryExport.h"
#include "base/messaging/messagePool.h"
#include "base/messaging/deviceMessage.h"

namespace dory
{
    class DORY_API DeviceListener
    {
        private:
            std::queue<std::shared_ptr<DeviceMessage>> messageBuffer;
            std::mutex mutex;

        public:
            virtual void handleMessage(std::shared_ptr<DeviceMessage> message);
            virtual void readUpdates(MessagePool& messagePool);
    };
}