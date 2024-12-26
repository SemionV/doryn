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
            repository->each([this](auto& window) {
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
            });
        });
    }

    void GlfwWindowSystemDevice::framebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        auto* device = static_cast<GlfwWindowSystemDevice*>(glfwGetWindowUserPointer(window));
        device->onWindowResize(window, width, height);
    }

    void GlfwWindowSystemDevice::onWindowResize(GLFWwindow* windowHandler, int width, int height)
    {
        _registry.get<repositories::IWindowRepository, resources::WindowSystem::glfw>([&](repositories::IWindowRepository* repository) {
            repository->scan([&](auto& window) {
                if(window.windowSystem == resources::WindowSystem::glfw)
                {
                    auto& glfwWindow = (resources::entities::GlfwWindow&)window;

                    if(glfwWindow.handler == windowHandler)
                    {
                        _registry.get<events::window::Bundle::IDispatcher>([&](auto* dispatcher) {
                            dispatcher->charge(events::window::Resize{ window.id, (unsigned int)width, (unsigned int)height, window.windowSystem });
                        });

                        return true;
                    }
                }

                return false;
            });
        });
    }

    void GlfwWindowSystemDevice::setupWindow(resources::entities::Window& window, const resources::WindowParameters& parameters)
    {
        assert(window.windowSystem == resources::WindowSystem::glfw);
        auto& glfwWindow = (resources::entities::GlfwWindow&)window;

        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
        glfwWindowHint(GLFW_SAMPLES, 8); //TODO: pass this parameter from outside!
        auto glfwWindowHandler = glfwCreateWindow((int)parameters.width, (int)parameters.height, parameters.title.c_str(), nullptr, nullptr);
        glfwWindow.handler = glfwWindowHandler;

        glfwMakeContextCurrent(glfwWindowHandler);

        glfwSwapInterval(0); //No VSync. TODO: pass this as a parameter

        glfwSetWindowUserPointer(glfwWindow.handler, this);
        glfwSetFramebufferSizeCallback(glfwWindow.handler, GlfwWindowSystemDevice::framebufferSizeCallback);
    }

    void GlfwWindowSystemDevice::closeWindow(const resources::entities::Window& window)
    {
        assert(window.windowSystem == resources::WindowSystem::glfw);
        auto& glfwWindow = (resources::entities::GlfwWindow&)window;
        glfwDestroyWindow(glfwWindow.handler);
    }

    void GlfwWindowSystemDevice::setCurrentWindow(const resources::entities::Window& window)
    {
        assert(window.windowSystem == resources::WindowSystem::glfw);
        auto& glfwWindow = (resources::entities::GlfwWindow&)window;

        glfwMakeContextCurrent(glfwWindow.handler);
    }

    void GlfwWindowSystemDevice::swapWindowBuffers(const resources::entities::Window& window)
    {
        assert(window.windowSystem == resources::WindowSystem::glfw);
        auto& glfwWindow = (resources::entities::GlfwWindow&)window;

        glfwSwapBuffers(glfwWindow.handler);
    }
}