#include "base/dependencies.h"
#include "device.h"

namespace dory
{
    void Device::attachListener(std::shared_ptr<DeviceListener> listener)
    {
        listeners.push_back(listener);
    }
}