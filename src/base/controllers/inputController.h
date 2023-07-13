#pragma once

#include "base/controller.h"
#include "base/devices/deviceListener.h"
#include "base/devices/device.h"
#include "base/messaging/messagePool.h"

namespace dory
{
    class DORY_API InputController: public dory::Controller
    {
        private:
            std::vector<DeviceListener*> _deviceListeners;
            std::vector<std::shared_ptr<DeviceListener>> deviceListeners;
            std::vector<std::shared_ptr<Device>> devices;
            MessagePool& messagePool;

        public:

        InputController(MessagePool& messagePool);

        void initialize(dory::DataContext& context);

        void stop(dory::DataContext& context);

        void update(const dory::TimeSpan& timeStep, dory::DataContext& context);

        void addDeviceListener(DeviceListener* device);
        void addDeviceListener(std::shared_ptr<DeviceListener> deviceListener);
        void addDevice(std::shared_ptr<Device> device);
    };
}
