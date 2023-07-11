#pragma once

#include "base/doryExport.h"
#include "base/messaging/messagePool.h"
#include "base/messaging/messages/consoleMessage.h"
#include "deviceListener.h"
#include "base/multithreading/individualProcessThread.h"
#include "base/multithreading/lambdaTask.h"

namespace dory
{
    class DORY_API SystemConsole: public DeviceListener
    {
        private:
            char inputKey;
            std::shared_ptr<IndividualProcessThread> individualThread;

        public:
            SystemConsole();

            bool connect();
            void disconnect();
            void readUpdates(MessagePool& messagePool);
            void onInput(int key);
        
        private:
            void bindStdHandlesToConsole();
    };
}