#pragma once

#include "base/doryExport.h"
#include "base/messaging/messagePool.h"
#include "device.h"

namespace dory
{
    class DORY_API SystemWindow: public Device
    {
        private:
            bool isStop;
            std::thread workingThread;

        public:
            SystemWindow();

            bool connect();
            void disconnect();
            void readUpdates(MessagePool& messagePool);
        
        private:
            void monitorSystemWindow();
    };
}