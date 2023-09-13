#include "base/dependencies.h"
#include "inputController.h"

namespace dory
{
    bool InputController::initialize(dory::DataContext& context)
    {
        return true;
    }

    void InputController::stop(dory::DataContext& context)
    {
    }

    void InputController::update(const dory::TimeSpan& timeStep, dory::DataContext& context)
    {
        auto it = devices.begin();
        auto end = devices.end();

        for(; it != end; ++it)
        {
            (*it)->update();
            (*it)->submitEvents(context);
        }
    }

    void InputController::addDevice(std::shared_ptr<IDevice> device)
    {
        devices.push_back(device);
    }
}