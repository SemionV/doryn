#include <dory/core/registry.h>
#include <dory/core/devices/glfwWindowSystemDevice.h>
#include <GLFW/glfw3.h>
#include <dory/core/repositories/iWindowRepository.h>
#include <dory/core/resources/glfwWindow.h>
#include <spdlog/fmt/fmt.h>

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

        auto logger = _registry.get<services::ILogService>();

        int monitorCount;
        GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

        for (int i = 0; i < monitorCount; i++)
        {
            // This is purely for debugging/logging
            const char* name = glfwGetMonitorName(monitors[i]);
            const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
            logger->information(fmt::format("Monitor #{0} ({1}): {2}x{3} @{4}Hz", i, name, mode->width, mode->height, mode->refreshRate));
        }

        GLFWmonitor* monitor;
        if(monitorCount > 1)
        {
            monitor = monitors[1];
        }
        else
        {
            monitor = glfwGetPrimaryMonitor();
        }

        if (!monitor)
        {
            if(logger)
            {
                logger->error(std::string_view{"Failed to get primary monitor."});
            }

            return;
        }

        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        if (!mode)
        {
            if(logger)
            {
                logger->error(std::string_view{"Failed to get monitor video mode."});
            }

            return;
        }


        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
        glfwWindowHint(GLFW_SAMPLES, parameters.sampling);

        GLFWwindow* glfwWindowHandler {};
        if(parameters.fullScreen)
        {
            glfwWindowHandler = glfwCreateWindow(mode->width, mode->height, parameters.title.c_str(), monitor, nullptr);
        }
        else
        {
            glfwWindowHandler = glfwCreateWindow((int)parameters.width, (int)parameters.height, parameters.title.c_str(),
                                                 nullptr, nullptr);
        }
        glfwWindow.handler = glfwWindowHandler;

        glfwMakeContextCurrent(glfwWindowHandler);

        glfwSwapInterval(parameters.vSync ? 1 : 0); //No VSync. TODO: pass this as a parameter

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