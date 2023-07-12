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

        std::vector<DeviceListener*>::iterator it2 = deviceListeners.begin();
        std::vector<DeviceListener*>::iterator end2 = deviceListeners.end();

        for(; it2 != end2; ++it2)
        {
            (*it2)->connect();
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

        std::vector<DeviceListener*>::iterator it2 = deviceListeners.begin();
        std::vector<DeviceListener*>::iterator end2 = deviceListeners.end();

        for(; it2 != end2; ++it2)
        {
            (*it2)->disconnect();
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

        std::vector<DeviceListener*>::iterator it2 = deviceListeners.begin();
        std::vector<DeviceListener*>::iterator end2 = deviceListeners.end();

        for(; it2 != end2; ++it2)
        {
            (*it2)->readUpdates(messagePool);
        }
    }

    void InputController::addDevice(DeviceListener* device)
    {
        deviceListeners.push_back(device);
    }

    void InputController::addDevice(std::shared_ptr<Device> device)
    {
        devices.push_back(device);
    }
}