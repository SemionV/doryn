#include <dory/core/devices/glfwDisplaySystemDevice.h>

#include <dory/core/resources/systemTypes.h>

namespace dory::core::devices
{
    GlfwDisplaySystemDevice::GlfwDisplaySystemDevice(Registry& registry): DependencyResolver(registry)
    {}

    void GlfwDisplaySystemDevice::connect(resources::DataContext& context)
    {
    }

    void GlfwDisplaySystemDevice::disconnect(resources::DataContext& context)
    {
    }
}
