#include <dory/core/registry.h>
#include <dory/core/services/windowService.h>
#include <dory/core/repositories/iViewRepository.h>
#include <dory/core/repositories/iWindowRepository.h>

namespace dory::core::services
{
    WindowService::WindowService(Registry& registry):
        _registry(registry)
    {}

    resources::IdType WindowService::initializeWindow(const resources::entities::Window& window)
    {
        resources::IdType id;

        _registry.get<repositories::IWindowRepository>([&window, &id](repositories::IWindowRepository* repository) {
            id = repository->insert(window);
        });

        _registry.get<IGraphicalSystem>(window.graphicalSystem, [&window](IGraphicalSystem* graphicalSystem) {
            graphicalSystem->initializeGraphics(window);
        });

        return id;
    }

    void WindowService::removeWindow(resources::IdType windowId)
    {
        _registry.get<repositories::IWindowRepository>([&windowId](repositories::IWindowRepository* repository) {
            repository->remove(windowId);
        });

        _registry.get<repositories::IViewRepository>([windowId](repositories::IViewRepository* viewRepository) {
            auto viewIds = viewRepository->getWindowViews(windowId);
            for(const auto viewId : viewIds)
            {
                viewRepository->remove(viewId);
            }
        });
    }
}