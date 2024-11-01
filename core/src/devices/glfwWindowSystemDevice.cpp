#include <dory/core/registry.h>
#include <dory/core/devices/glfwWindowSystemDevice.h>
#include <GLFW/glfw3.h>
#include <dory/core/repositories/iRepository.h>
#include <dory/core/resources/window.h>

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

        _registry.get<repositories::IRepository<resources::entity::Window>>([this, &context](repositories::IRepository<resources::entity::Window>* repository) {
            auto windows = repository->getAll();
            for(auto& window : windows)
            {
                if(window.windowSystemType == resources::WindowSystem::glfw)
                {
                    auto specificData = std::static_pointer_cast<resources::entity::GlfwWindow>(window.subsystemData);

                    if(glfwWindowShouldClose(specificData->handler))
                    {
                        _registry.get<events::window::Bundle::IDispatcher>([&window](auto* dispatcher) {
                            dispatcher->charge(events::window::Close{ window.id, window.windowSystemType });
                        });

                        glfwSetWindowShouldClose(specificData->handler, 0);
                    }
                }
            }

            _registry.get<events::window::Bundle::IDispatcher>([&context](auto* dispatcher) {
                dispatcher->fireAll(context);
            });
        });
    }
}