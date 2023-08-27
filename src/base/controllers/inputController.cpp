#include "base/dependencies.h"
#include "inputController.h"

namespace dory
{
    InputController::InputController(std::shared_ptr<SystemConsoleEventHubDispatcher> consoleEventHub,
        std::shared_ptr<SystemWindowEventHubDispatcher> windowEventHub):
        consoleEventHub(consoleEventHub),
        windowEventHub(windowEventHub)
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

        consoleEventHub->submit(context);
        windowEventHub->submit(context);
    }

    void InputController::addDevice(std::shared_ptr<Device> device)
    {
        devices.push_back(device);
    }
}