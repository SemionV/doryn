#pragma once

#include "base/doryExport.h"
#include "base/messaging/messagePool.h"
#include "deviceListener.h"

namespace dory
{
    class DORY_API SystemWindow: public DeviceListener
    {
        private:
            int clickX, clickY;

        public:
            SystemWindow();

            bool connect();
            void disconnect();
            void readUpdates(MessagePool& messagePool);
            void addClickMessage(int clickX, int clickY);

        private:
            int createWindow();
    };
}