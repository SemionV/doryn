#pragma once

#include "base/controller.h"
#include "base/devices/deviceListener.h"
#include "base/devices/device.h"
#include "base/messaging/messagePool.h"
#include "base/messaging/events/systemConsole/systemConsoleEventHub.h"

namespace dory
{
    class DORY_API InputController: public dory::Controller
    {
        private:
            std::vector<std::shared_ptr<DeviceListener>> deviceListeners;
            std::vector<std::shared_ptr<Device>> devices;
            MessagePool& messagePool;
            std::shared_ptr<SystemConsoleEventHubDispatcher> consoleEventHub;

        public:

        InputController(MessagePool& messagePool, std::shared_ptr<SystemConsoleEventHubDispatcher> consoleEventHub);

        void initialize(dory::DataContext& context);
        void stop(dory::DataContext& context);
        void update(const dory::TimeSpan& timeStep, dory::DataContext& context);

        void addDeviceListener(std::shared_ptr<DeviceListener> deviceListener);
        void addDevice(std::shared_ptr<Device> device);
    };
}
