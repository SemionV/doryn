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
    }

    void InputController::stop(dory::DataContext& context)
    {
    }

    void InputController::update(const dory::TimeSpan& timeStep, dory::DataContext& context)
    {
        std::vector<std::shared_ptr<IDevice>>::iterator it = devices.begin();
        std::vector<std::shared_ptr<IDevice>>::iterator end = devices.end();

        for(; it != end; ++it)
        {
            (*it)->update();
        }

        consoleEventHub->submit(context);
        windowEventHub->submit(context);
    }

    void InputController::addDevice(std::shared_ptr<IDevice> device)
    {
        devices.push_back(device);
    }
}