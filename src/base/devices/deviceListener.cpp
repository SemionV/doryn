#include "base/dependencies.h"
#include "deviceListener.h"

namespace dory
{
    void DeviceListener::handleMessage(std::shared_ptr<DeviceMessage> message)
    {
        const std::lock_guard<std::mutex> lock(mutex);

        messageBuffer.push(message);
    }

    void DeviceListener::readUpdates(MessagePool& messagePool)
    {
        while(!messageBuffer.empty())
        {
            std::shared_ptr<DeviceMessage> message = messageBuffer.front();
            messagePool.addMessage(message);
            messageBuffer.pop();
        }
    }

    bool DeviceListener::connect()
    {        
    }

    void DeviceListener::disconnect()
    {        
    }
}