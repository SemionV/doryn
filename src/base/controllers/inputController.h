#pragma once

#include "base/controller.h"
#include "base/devices/device.h"
#include "base/messagePool.h"

namespace dory
{
    class DORY_API InputController: public dory::Controller
    {
        private:
            std::vector<Device*> devices;
            MessagePool& messagePool;

        public:

        InputController(MessagePool& messagePool);

        void initialize(dory::DataContext& context);

        void stop(dory::DataContext& context);

        void update(const dory::TimeSpan& timeStep, dory::DataContext& context);

        void addDevice(Device* device);
    };
}
