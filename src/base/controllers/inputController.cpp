#include "base/dependencies.h"
#include "inputController.h"

namespace dory
{
    InputController::InputController(MessagePool& messagePool):
        deviceListeners(),
        messagePool(messagePool)
    {
    }

    void InputController::initialize(dory::DataContext& context)
    {
        std::vector<std::shared_ptr<Device>>::iterator it = devices.begin();
        std::vector<std::shared_ptr<Device>>::iterator end = devices.end();

        for(; it != end; ++it)
        {
            (*it)->connect();
        }
    }

    void InputController::stop(dory::DataContext& context)
    {
        std::vector<std::shared_ptr<Device>>::iterator it = devices.begin();
        std::vector<std::shared_ptr<Device>>::iterator end = devices.end();

        for(; it != end; ++it)
        {
            (*it)->disconnect();
        }
    }

    void InputController::update(const dory::TimeSpan& timeStep, dory::DataContext& context)
    {
        std::vector<std::shared_ptr<Device>>::iterator it = devices.begin();
        std::vector<std::shared_ptr<Device>>::iterator end = devices.end();

        for(; it != end; ++it)
        {
            (*it)->update();
        }

        std::size_t size = deviceListeners.size();
        for(std::size_t i = 0; i < size; i++)
        {
            std::shared_ptr<DeviceListener> deviceListener = deviceListeners[i];

            deviceListener->readUpdates(messagePool);
        }
    }

    void InputController::addDeviceListener(std::shared_ptr<DeviceListener> deviceListener)
    {
        deviceListeners.push_back(deviceListener);
    }

    void InputController::addDevice(std::shared_ptr<Device> device)
    {
        devices.push_back(device);
    }
}