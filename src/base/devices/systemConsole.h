#pragma once

#include "base/doryExport.h"
#include "base/messagePool.h"
#include "device.h"

namespace dory
{
    class DORY_API SystemConsole: public Device
    {
        public:
            bool connect();
            void disconnect();
            void readUpdates(MessagePool& messagePool);
    };
}