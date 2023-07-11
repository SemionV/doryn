#include "base/dependencies.h"
#include "inputController.h"

namespace dory
{
    InputController::InputController(MessagePool& messagePool):
        devices(),
        messagePool(messagePool)
    {
    }

    void InputController::initialize(dory::DataContext& context)
    {
        std::vector<DeviceListener*>::iterator it = devices.begin();
        std::vector<DeviceListener*>::iterator end = devices.end();

        for(; it != end; ++it)
        {
            (*it)->connect();
        }
    }

    void InputController::stop(dory::DataContext& context)
    {
        std::vector<DeviceListener*>::iterator it = devices.begin();
        std::vector<DeviceListener*>::iterator end = devices.end();

        for(; it != end; ++it)
        {
            (*it)->disconnect();
        }
    }

    void InputController::update(const dory::TimeSpan& timeStep, dory::DataContext& context)
    {
        std::vector<DeviceListener*>::iterator it = devices.begin();
        std::vector<DeviceListener*>::iterator end = devices.end();

        for(; it != end; ++it)
        {
            (*it)->readUpdates(messagePool);
        }
    }

    void InputController::addDevice(DeviceListener* device)
    {
        devices.push_back(device);
    }
}