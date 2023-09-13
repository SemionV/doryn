#pragma once

#include "base/doryExport.h"

namespace dory
{
    class DORY_API IDevice
    {
        public:
            virtual bool connect() = 0;
            virtual void disconnect() = 0;
            virtual void update() = 0;
            virtual void submitEvents(DataContext& context) = 0;
    };
}