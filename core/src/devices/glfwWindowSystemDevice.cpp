#include <dory/core/registry.h>
#include <dory/core/devices/glfwWindowSystemDevice.h>
#include <GLFW/glfw3.h>
#include <dory/core/repositories/iWindowRepository.h>
#include <dory/core/resources/glfwWindow.h>

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

        _registry.get<repositories::IWindowRepository, resources::WindowSystem::glfw>([this, &context](repositories::IWindowRepository* repository) {
            auto windows = repository->getAll();
            for(auto& window : windows)
            {
                if(window.windowSystem == resources::WindowSystem::glfw)
                {
                    auto& glfwWindow = (resources::entities::GlfwWindow&)window;

                    if(glfwWindowShouldClose(glfwWindow.handler))
                    {
                        _registry.get<events::window::Bundle::IDispatcher>([&window](auto* dispatcher) {
                            dispatcher->charge(events::window::Close{ window.id, window.windowSystem });
                        });

                        glfwSetWindowShouldClose(glfwWindow.handler, 0);
                    }
                }
            }

            _registry.get<events::window::Bundle::IDispatcher>([&context](auto* dispatcher) {
                dispatcher->fireAll(context);
            });
        });
    }
}