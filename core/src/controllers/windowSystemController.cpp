#include <dory/core/registry.h>
#include <dory/core/controllers/windowSystemController.h>
#include <dory/core/devices/iWindowSystemDevice.h>
#include <dory/core/resources/windowSystem.h>

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
        _registry.get<devices::IWindowSystemDevice, resources::WindowSystem::glfw>([&context](devices::IWindowSystemDevice* device){
            device->pollEvents(context);
        });

        _registry.get<devices::IWindowSystemDevice, resources::WindowSystem::win32>([&context](devices::IWindowSystemDevice* device){
            device->pollEvents(context);
        });
    }
}