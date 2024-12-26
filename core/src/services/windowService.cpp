#include <dory/core/registry.h>
#include <dory/core/services/windowService.h>
#include <dory/core/repositories/iWindowRepository.h>

namespace dory::core::services
{
    WindowService::WindowService(Registry &registry) : DependencyResolver(registry)
    {}

    resources::entities::Window* WindowService::createWindow(const resources::WindowParameters& parameters,
                                                             const resources::WindowSystem windowSystem)
    {
        resources::entities::Window* window = nullptr;

        auto windowRepo = _registry.get<repositories::IWindowRepository>(windowSystem);
        auto windowSystemDevice = _registry.get<devices::IWindowSystemDevice>(windowSystem);
        if(windowRepo && windowSystemDevice)
        {
            window = windowRepo->insert(resources::entities::Window{ {}, windowSystem, parameters.graphicalContextId });
            if(window)
            {
                windowSystemDevice->setupWindow(*window, parameters);
            }
        }

        return window;
    }

    void WindowService::closeWindow(resources::IdType windowId, const resources::WindowSystem windowSystem)
    {
        auto windowRepo = _registry.get<repositories::IWindowRepository>(windowSystem);
        auto viewRepo = _registry.get<repositories::IViewRepository>();
        auto windowSystemDevice = _registry.get<devices::IWindowSystemDevice>(windowSystem);
        if(windowRepo && viewRepo && windowSystemDevice)
        {
            auto window = windowRepo->get(windowId);
            if(window)
            {
                windowSystemDevice->closeWindow(*window);

                for(const auto viewId : window->views)
                {
                    viewRepo->remove(viewId);
                }

                windowRepo->remove(windowId);
            }
        }
    }

    void WindowService::setCurrentWindow(const resources::entities::Window& window)
    {
        auto windowSystemDevice = _registry.get<devices::IWindowSystemDevice>(window.windowSystem);
        if(windowSystemDevice)
        {
            windowSystemDevice->setCurrentWindow(window);
        }
    }

    void WindowService::swapBuffers(const resources::entities::Window& window)
    {
        auto windowSystemDevice = _registry.get<devices::IWindowSystemDevice>(window.windowSystem);
        if(windowSystemDevice)
        {
            windowSystemDevice->swapWindowBuffers(window);
        }
    }
}