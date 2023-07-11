#pragma once

#include "base/doryExport.h"
#include "base/messaging/messagePool.h"
#include "base/messaging/messages/consoleMessage.h"
#include "deviceListener.h"
#include "base/multithreading/systemThread.h"
#include "base/multithreading/lambdaTask.h"
#include "readConsoleInputTask.h"

namespace dory
{
    class ReadConsoleInputTask;

    class DORY_API SystemConsole: public DeviceListener
    {
        private:
            char inputKey;
            SystemThread* systemThread;
            LambdaTask* readInputTask;

        public:
            SystemConsole();
            ~SystemConsole();

            bool connect();
            void disconnect();
            void readUpdates(MessagePool& messagePool);
            void onInput(int key);
        
        private:
            void bindStdHandlesToConsole();
    };
}