#include "base/dependencies.h"
#include "device.h"

namespace dory
{
    void Device::attachListener(std::shared_ptr<DeviceListener> listener)
    {
        listeners.push_back(listener);
    }

    void Device::propagateMessage(std::shared_ptr<DeviceMessage> message)
    {
        std::size_t size = listeners.size();
        for(std::size_t i = 0; i < size; i++)
        {
            listeners[i]->handleMessage(message);
        }
    }
}