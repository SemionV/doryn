#pragma once

#include "base/controller.h"
#include "base/devices/device.h"
#include "base/messaging/events/systemConsole/systemConsoleEventHub.h"
#include "base/messaging/events/systemWindow/systemWindowEventHub.h"

namespace dory
{
    class DORY_API InputController: public dory::Controller
    {
        private:
            std::vector<std::shared_ptr<Device>> devices;
            std::shared_ptr<SystemConsoleEventHubDispatcher> consoleEventHub;
            std::shared_ptr<SystemWindowEventHubDispatcher> windowEventHub;

        public:

        InputController(std::shared_ptr<SystemConsoleEventHubDispatcher> consoleEventHub,
            std::shared_ptr<SystemWindowEventHubDispatcher> windowEventHub);

        void initialize(dory::DataContext& context);
        void stop(dory::DataContext& context);
        void update(const dory::TimeSpan& timeStep, dory::DataContext& context);

        void addDevice(std::shared_ptr<Device> device);
    };
}
