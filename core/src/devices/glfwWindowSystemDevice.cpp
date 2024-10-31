#include <dory/core/registry.h>
#include <dory/core/devices/glfwWindowSystemDevice.h>
#include <GLFW/glfw3.h>
#include <dory/core/repositories/iRepository.h>

namespace dory::core::devices
{
    GlfwWindowSystemDevice::GlfwWindowSystemDevice(Registry &registry):
        _registry(registry)
    {}

    void GlfwWindowSystemDevice::connect(resources::DataContext& context)
    {
        glfwInit();
    }

    void GlfwWindowSystemDevice::disconnect(resources::DataContext& context)
    {
        glfwTerminate();
    }

    void GlfwWindowSystemDevice::pollEvents(resources::DataContext& context)
    {
        glfwPollEvents();

        /*_registry.get<repositories::IRepository<resources::entity::Window>>([](repositories::IRepository<resources::entity::Window>* repository) {
            repository.
        });*/

        /*windowRepository.forEach([this](auto& window)
        {
            if(glfwWindowShouldClose(window.handler))
            {
                this->windowEventHubDispatcher.charge(events::window::Close(window.id));
                glfwSetWindowShouldClose(window.handler, 0);
            }
        });

        windowEventHubDispatcher.fireAll(context);*/
    }
}