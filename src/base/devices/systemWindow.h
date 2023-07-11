#pragma once

#include "base/doryExport.h"
#include "base/messaging/messagePool.h"
#include "deviceListener.h"
#include "base/multithreading/individualProcessThread.h"
#include "base/multithreading/currentProcessThread.h"
#include "base/multithreading/lambdaTask.h"

namespace dory
{
    class DORY_API SystemWindow: public DeviceListener
    {
        private:
            int clickX, clickY;
            std::shared_ptr<ProcessThread> processThread;
            std::mutex mutex;

        public:
            SystemWindow();

            bool connect();
            void disconnect();
            void readUpdates(MessagePool& messagePool);
            void handleMessage(std::shared_ptr<DeviceMessage> message);
            void addClickMessage(int clickX, int clickY);

        private:
            int createWindow();
    };
}