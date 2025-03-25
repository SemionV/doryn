#include <dory/core/devices/glfwDisplaySystemDevice.h>

#include <GLFW/glfw3.h>
#include <dory/core/resources/systemTypes.h>
#include <dory/core/resources/glfwDisplay.h>
#include "dory/core/registry.h"

namespace dory::core::devices
{
    struct GlfwMonitorList
    {
        int monitorCount {};
        GLFWmonitor** monitors {};
        GLFWmonitor* primaryMonitor {};
    };

    GlfwMonitorList getMonitorList()
    {
        GlfwMonitorList list;

        list.primaryMonitor = glfwGetPrimaryMonitor();
        list.monitors = glfwGetMonitors(&list.monitorCount);

        return list;
    }

    bool isDisplaySettingsChanged(GLFWmonitor* glfwMonitor, GLFWmonitor* primaryMonitor, const resources::entities::GlfwDisplay display)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwMonitor);

        return !mode || (display.size.width != mode->width
            || display.size.height != mode->height
            || display.refreshRate != mode->refreshRate
            || (display.isPrimary && display.glfwMonitor != primaryMonitor)
            || (!display.isPrimary && display.glfwMonitor == primaryMonitor));
    }

    void setupDisplay(GLFWmonitor* glfwMonitor, const GLFWmonitor* primaryMonitor, resources::entities::GlfwDisplay& display)
    {
        const char* name = glfwGetMonitorName(glfwMonitor);
        const GLFWvidmode* mode = glfwGetVideoMode(glfwMonitor);

        display.glfwMonitor = glfwMonitor;
        display.name = name;
        if(mode)
        {
            display.size.width = mode->width;
            display.size.height = mode->height;
            display.refreshRate = mode->refreshRate;
        }
        display.isPrimary = glfwMonitor == primaryMonitor;
    }

    void GlfwDisplaySystemDevice::updateDisplays() const
    {
        auto eventHub = _registry.get<events::display::Bundle::IDispatcher>();
        auto displayRepo = _registry.get<repositories::IDisplayRepository, resources::DisplaySystem::glfw>();
        const auto monitorList = getMonitorList();
        if(displayRepo && eventHub)
        {
            //find all disconnected displays
            displayRepo->each([&monitorList, &eventHub](auto& display)
            {
                bool monitorConnected {};
                for(std::size_t i = 0; i < monitorList.monitorCount; ++i)
                {
                    const GLFWmonitor* monitor = monitorList.monitors[i];
                    if(display.displaySystem == resources::DisplaySystem::glfw)
                    {
                        const auto& glfwDisplay = static_cast<resources::entities::GlfwDisplay&>(display);
                        if(glfwDisplay.glfwMonitor == monitor)
                        {
                            monitorConnected = true;
                            break;
                        }
                    }
                }

                if(!monitorConnected)
                {
                    eventHub->charge(events::display::DisplayDisconnected{ display.id });
                }
            });

            //find all connected or modified displays
            for(std::size_t i = 0; i < monitorList.monitorCount; ++i)
            {
                GLFWmonitor* glfwMonitor = monitorList.monitors[i];

                auto* display = (resources::entities::GlfwDisplay*)displayRepo->scan([glfwMonitor](auto& display)
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

                if(!display)
                {
                    display = (resources::entities::GlfwDisplay*)displayRepo->create();
                    if(display)
                    {
                        eventHub->charge(events::display::DisplayConnected{ display->id });
                        setupDisplay(glfwMonitor, monitorList.primaryMonitor, *display);
                    }
                }
                else if(isDisplaySettingsChanged(glfwMonitor, monitorList.primaryMonitor, *display))
                {
                    eventHub->charge(events::display::DisplayChanged{ display->id });
                    setupDisplay(glfwMonitor, monitorList.primaryMonitor, *display);
                }
            }
        }
    }

    GlfwDisplaySystemDevice::GlfwDisplaySystemDevice(Registry& registry): DependencyResolver(registry)
    {}

    void GlfwDisplaySystemDevice::connect(resources::DataContext& context)
    {
        glfwInit();
        updateDisplays();
    }

    void GlfwDisplaySystemDevice::disconnect(resources::DataContext& context)
    {
        glfwTerminate();
    }
}
