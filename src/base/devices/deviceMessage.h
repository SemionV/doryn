#pragma once

#include "base/doryExport.h"

namespace dory
{
    enum DORY_API MessageType
    {

    };

    struct DORY_API DeviceMessage
    {
        public:
            MessageType messageType;
    };
}