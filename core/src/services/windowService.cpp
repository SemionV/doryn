#include <dory/core/registry.h>
#include <dory/core/services/windowService.h>
#include <dory/core/repositories/iViewRepository.h>
#include <dory/core/repositories/iWindowRepository.h>
#include <dory/core/repositories/iGraphicalContextRepository.h>
#include <dory/core/resources/entities/graphicalContext.h>

namespace dory::core::services
{
    WindowService::WindowService(Registry& registry):
        _registry(registry)
    {}

    resources::entities::Window& WindowService::addWindow(resources::entities::Window& window)
    {
        resources::entities::Window* newWindow = nullptr;

        _registry.get<repositories::IWindowRepository>([&window, &newWindow](repositories::IWindowRepository* repository) {
            newWindow = repository->insert(window);
        });

        return *newWindow;
    }

    void WindowService::removeWindow(resources::IdType windowId)
    {
        _registry.get<repositories::IWindowRepository>([&](repositories::IWindowRepository* repository) {
            auto window = repository->get(windowId);

            if(window)
            {
                auto viewRepository = _registry.get<repositories::IViewRepository>();
                if(viewRepository)
                {
                    for(const auto viewId : window->views)
                    {
                        viewRepository->remove(viewId);
                    }
                }
            }

            _registry.get<repositories::IGraphicalContextRepository>([window](repositories::IGraphicalContextRepository* graphicalContextRepository) {
                auto graphicalContext = graphicalContextRepository->scan([window](auto& x) {
                    return x.id == window->graphicalContextId;
                });

                if(graphicalContext)
                {
                    graphicalContextRepository->remove(graphicalContext->id);
                }
            });

            _registry.get<repositories::IWindowRepository>([windowId](repositories::IWindowRepository* repository) {
                repository->remove(windowId);
            });
        });
    }
}