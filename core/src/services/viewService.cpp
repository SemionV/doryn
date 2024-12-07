#include <dory/core/registry.h>
#include <dory/core/services/viewService.h>

namespace dory::core::services
{
    ViewService::ViewService(Registry& registry) : DependencyResolver(registry)
    {}

    void ViewService::updateViews(resources::DataContext& context)
    {
        auto viewRepository = _registry.get<repositories::IViewRepository>();
        auto windowRepository = _registry.get<repositories::IWindowRepository>();
        auto graphicalContextRepository = _registry.get<repositories::IGraphicalContextRepository>();
        auto renderer = _registry.get<graphics::IRenderer>();

        if(viewRepository && windowRepository && graphicalContextRepository && renderer)
        {
            viewRepository->each([&context, &renderer, &windowRepository, &graphicalContextRepository](resources::entities::View& view) {
                auto window = windowRepository->get(view.windowId);
                if(window)
                {
                    auto graphicalContext = graphicalContextRepository->get(window->graphicalContextId);
                    if(graphicalContext)
                    {
                        renderer->draw(context, *window, *graphicalContext, view);
                    }
                }
            });
        }
    }
}