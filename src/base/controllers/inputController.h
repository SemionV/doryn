#pragma once

#include "base/controller.h"
#include "base/devices/device.h"
#include "base/messaging/events/systemConsole/systemConsoleEventHub.h"
#include "base/messaging/events/systemWindow/systemWindowEventHub.h"

namespace dory
{
    class DORY_API InputController: public Controller
    {
        private:
            std::vector<std::shared_ptr<IDevice>> devices;
            std::shared_ptr<SystemConsoleEventHubDispatcher> consoleEventHub;
            std::shared_ptr<SystemWindowEventHubDispatcher> windowEventHub;

        public:

        InputController(std::shared_ptr<SystemConsoleEventHubDispatcher> consoleEventHub,
            std::shared_ptr<SystemWindowEventHubDispatcher> windowEventHub);

        bool initialize(dory::DataContext& context) override;
        void stop(dory::DataContext& context) override;
        void update(const dory::TimeSpan& timeStep, dory::DataContext& context) override;

        void addDevice(std::shared_ptr<IDevice> device);
    };
}
