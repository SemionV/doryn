#include <dory/core/registry.h>
#include <dory/core/devices/glfwWindowSystemDevice.h>
#include <GLFW/glfw3.h>
#include <dory/core/repositories/iWindowRepository.h>
#include <dory/core/resources/glfwWindow.h>
#include <dory/core/resources/glfwDisplay.h>
#include <dory/core/resources/profiling.h>
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
        resources::profiling::pushTimeSlice(context.profiling, "GlfwWindowSystemDevice::pollEvents");

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

        resources::profiling::popTimeSlice(context.profiling);
    }

    resources::entities::GlfwWindow* GlfwWindowSystemDevice::getWindow(Registry& registry, GLFWwindow* windowHandler)
    {
        resources::entities::GlfwWindow* result = nullptr;

        if(auto windowRepo = registry.get<repositories::IWindowRepository, resources::WindowSystem::glfw>())
        {
            windowRepo->scan([&](auto& window) {
                if(window.windowSystem == resources::WindowSystem::glfw)
                {
                    auto& glfwWindow = (resources::entities::GlfwWindow&)window;

                    if(glfwWindow.handler == windowHandler)
                    {
                        result = (resources::entities::GlfwWindow*)&window;
                        return true;
                    }
                }

                return false;
            });
        }

        return result;
    }

    events::KeyCode GlfwWindowSystemDevice::getKeyCode(int glfwKey)
    {
        switch(glfwKey)
        {
        case GLFW_KEY_ESCAPE: return events::KeyCode::Escape;
        case GLFW_KEY_UP: return events::KeyCode::Up;
        case GLFW_KEY_DOWN: return events::KeyCode::Down;
        case GLFW_KEY_LEFT: return events::KeyCode::Left;
        case GLFW_KEY_RIGHT: return events::KeyCode::Right;
        case GLFW_KEY_W: return events::KeyCode::W;
        case GLFW_KEY_A: return events::KeyCode::A;
        case GLFW_KEY_S: return events::KeyCode::S;
        case GLFW_KEY_D: return events::KeyCode::D;
        default: return events::KeyCode::Unknown;
        }
    }

    events::KeyAction GlfwWindowSystemDevice::getKeyAction(int action)
    {
        switch(action)
        {
            case GLFW_PRESS: return events::KeyAction::Press;
            case GLFW_RELEASE: return events::KeyAction::Release;
            case GLFW_REPEAT: return events::KeyAction::Repeat;
            default: return events::KeyAction::Unknown;
        }
    }

    events::ModificationKeysState GlfwWindowSystemDevice::getModKeysState(int mods)
    {
        events::ModificationKeysState modKeysState;

        modKeysState.altKey = (mods & GLFW_MOD_ALT) != 0;
        modKeysState.ctrlKey = (mods & GLFW_MOD_CONTROL) != 0;
        modKeysState.shiftKey = (mods & GLFW_MOD_SHIFT) != 0;
        modKeysState.superKey = (mods & GLFW_MOD_SUPER) != 0;

        return modKeysState;
    }

    void GlfwWindowSystemDevice::framebufferSizeCallback(GLFWwindow* windowHandler, const int width, const int height)
    {
        auto* registry = static_cast<Registry*>(glfwGetWindowUserPointer(windowHandler));
        const auto* window = getWindow(*registry, windowHandler);

        registry->get<events::window::Bundle::IDispatcher>([&](auto* dispatcher) {
            dispatcher->charge(events::window::Resize{ window->id, window->windowSystem, (unsigned int)width, (unsigned int)height });
        });
    }

    void GlfwWindowSystemDevice::keyCallback(GLFWwindow* windowHandler, const int key, int scancode, int action, int mods)
    {
        auto* registry = static_cast<Registry*>(glfwGetWindowUserPointer(windowHandler));
        const auto* window = getWindow(*registry, windowHandler);

        registry->get<events::window::Bundle::IDispatcher>([&](auto* dispatcher) {
            dispatcher->charge(events::window::KeyboardEvent{
                window->id,
                window->windowSystem,
                getKeyCode(key),
                getKeyAction(action),
                getModKeysState(mods)
            });
        });
    }

    void GlfwWindowSystemDevice::cursorPosCallback(GLFWwindow* windowHandler, double x, double y)
    {

    }

    void GlfwWindowSystemDevice::mouseButtonCallback(GLFWwindow* windowHandler, int button, int action, int mods)
    {

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
            monitor = monitors[0];
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

        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE); //TODO: pass this as a parameter
        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
        if(parameters.sampling)
        {
            glfwWindowHint(GLFW_SAMPLES, parameters.sampling);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* glfwWindowHandler {};
        if(parameters.fullScreen)
        {
            glfwWindowHandler = glfwCreateWindow(mode->width, mode->height, parameters.title.c_str(), monitor, nullptr);
            window.width = mode->width;
            window.height = mode->height;
        }
        else
        {
            glfwWindowHandler = glfwCreateWindow((int)parameters.width, (int)parameters.height, parameters.title.c_str(),
                                                 nullptr, nullptr);
            window.width = parameters.width;
            window.height = parameters.height;
        }
        glfwWindow.handler = glfwWindowHandler;

        glfwMakeContextCurrent(glfwWindowHandler);

        glfwSwapInterval(parameters.vSync ? 1 : 0);
        glfwSetWindowUserPointer(glfwWindow.handler, &_registry);
        glfwSetFramebufferSizeCallback(glfwWindow.handler, framebufferSizeCallback);
        glfwSetKeyCallback(glfwWindow.handler, keyCallback);
        glfwSetCursorPosCallback(glfwWindow.handler, cursorPosCallback);
        glfwSetMouseButtonCallback(glfwWindow.handler, mouseButtonCallback);
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

    void GlfwWindowSystemDevice::loadDisplays()
    {
        if(auto displayRepository = _registry.get<repositories::IDisplayRepository, resources::DisplaySystem::glfw>())
        {
            int monitorCount;
            GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
            const GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();

            for (int i = 0; i < monitorCount; i++)
            {
                const auto glfwMonitor = monitors[i];

                auto* glfwDisplay = displayRepository->scan([glfwMonitor](auto& display)
                {
                    if(display.displaySystem == resources::DisplaySystem::glfw)
                    {
                        const auto& glfwDisplay = static_cast<resources::entities::GlfwDisplay&>(display);
                        if(glfwDisplay.glfwMonitor == glfwMonitor)
                        {
                            return true;
                        }
                    }
                    return false;
                });

                if(!glfwDisplay)
                {
                    glfwDisplay = displayRepository->create();
                }
                else
                {
                    //TODO: check if display's resolution has changed
                }

                const char* name = glfwGetMonitorName(glfwMonitor);
                const GLFWvidmode* mode = glfwGetVideoMode(glfwMonitor);

                glfwDisplay->name = name;
                if(mode)
                {
                    glfwDisplay->size.width = mode->width;
                    glfwDisplay->size.height = mode->height;
                    glfwDisplay->refreshRate = mode->refreshRate;
                }

                glfwDisplay->isPrimary = glfwMonitor == primaryMonitor;
            }
        }
    }
}
