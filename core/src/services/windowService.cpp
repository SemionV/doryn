#include <dory/core/registry.h>
#include <dory/core/services/windowService.h>
#include <dory/core/repositories/iViewRepository.h>
#include <dory/core/repositories/iWindowRepository.h>

namespace dory::core::services
{
    WindowService::WindowService(Registry& registry):
        _registry(registry)
    {}

    resources::IdType WindowService::addWindow(const resources::entities::Window& window)
    {
        resources::IdType id = resources::nullId;

        _registry.get<repositories::IWindowRepository>([&window, &id](repositories::IWindowRepository* repository) {
            auto* windowEntity = repository->insert(window);
            if(windowEntity)
            {
                id = windowEntity->id;
            }
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