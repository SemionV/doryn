#pragma once

#include "base/doryExport.h"
#include "deviceListener.h"

namespace dory
{
    class DORY_API Device
    {
        private:
            std::vector<std::shared_ptr<DeviceListener>> listeners;

        public:
            virtual bool connect() = 0;
            virtual void disconnect() = 0;
            virtual void update() = 0;
            virtual void attachListener(std::shared_ptr<DeviceListener> listener);
        
        protected:
            virtual void propagateMessage(std::shared_ptr<DeviceMessage> message);
    };
}