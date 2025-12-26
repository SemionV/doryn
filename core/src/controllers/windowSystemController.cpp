#include <dory/core/registry.h>
#include <dory/core/controllers/windowSystemController.h>
#include <dory/core/devices/iWindowSystemDevice.h>
#include <dory/core/resources/systemTypes.h>

namespace dory::core::controllers
{
    WindowSystemController::WindowSystemController(Registry &registry):
        _registry(registry)
    {}

    bool WindowSystemController::initialize(resources::IdType referenceId, resources::DataContext& context)
    {
        return true;
    }

    void WindowSystemController::stop(resources::IdType referenceId, resources::DataContext& context)
    {}

    void WindowSystemController::update(resources::IdType referenceId, const generic::model::TimeSpan& timeStep, resources::DataContext& context)
    {
        _registry.getAll<devices::IWindowSystemDevice>([&context](resources::WindowSystem windowSystem, devices::IWindowSystemDevice* device) {
            device->pollEvents(context);
        });
    }
}