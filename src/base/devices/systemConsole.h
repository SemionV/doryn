#pragma once

#include "base/doryExport.h"
#include "base/messaging/messagePool.h"
#include "base/messaging/messages/consoleMessage.h"
#include "device.h"

namespace dory
{
    class DORY_API SystemConsole: public Device
    {
        private:
            bool isStop;
            char inputKey;
            std::thread workingThread;

        public:
            SystemConsole();

            bool connect();
            void disconnect();
            void readUpdates(MessagePool& messagePool);
        
        private:
            void monitorSystemConsole();
            void bindStdHandlesToConsole();
    };
}