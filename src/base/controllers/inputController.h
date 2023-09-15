#pragma once

#include "base/controller.h"
#include "base/devices/device.h"
#include "base/messaging/events/systemConsole/systemConsoleEventHub.h"

namespace dory
{
    class DORY_API InputController: public Controller
    {
        private:
            std::vector<std::shared_ptr<IDevice>> devices;

        public:
        bool initialize(dory::DataContext& context) override;
        void stop(dory::DataContext& context) override;
        void update(const dory::TimeSpan& timeStep, dory::DataContext& context) override;

        void addDevice(std::shared_ptr<IDevice> device);
    };
}
