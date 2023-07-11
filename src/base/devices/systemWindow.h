#pragma once

#include "base/doryExport.h"
#include "base/messaging/messagePool.h"
#include "deviceListener.h"
#include "base/multithreading/systemThread.h"
#include "base/multithreading/lambdaTask.h"

namespace dory
{
    class DORY_API SystemWindow: public DeviceListener
    {
        private:
            int clickX, clickY;
            SystemThread* systemThread;
            LambdaTask* pumpMessagesTask;
            std::mutex mutex;

        public:
            SystemWindow();
            ~SystemWindow();

            bool connect();
            void disconnect();
            void readUpdates(MessagePool& messagePool);
            void addClickMessage(int clickX, int clickY);

        private:
            int createWindow();
    };
}