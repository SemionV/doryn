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

    resources::entities::View* ViewService::createView(resources::entities::Window& window)
    {
        resources::entities::View* view;

        _registry.get<dory::core::repositories::IViewRepository>([&](dory::core::repositories::IViewRepository* viewRepository) {
            view = viewRepository->insert(resources::entities::View{{}, window.id});
            if(view != nullptr)
            {
                window.views.push_back(view->id);
            }
        });

        return view;
    }

    void ViewService::destroyView(resources::IdType viewId)
    {
        _registry.get<dory::core::repositories::IViewRepository>([&](dory::core::repositories::IViewRepository* viewRepository) {
            auto view = viewRepository->get(viewId);
            if(view)
            {
                auto windowRepository = _registry.get<repositories::IWindowRepository>();
                if(windowRepository)
                {
                    auto window = windowRepository->get(view->windowId);
                    if(window)
                    {
                        auto item = std::find(window->views.begin(), window->views.end(), view->windowId);
                        if(item != window->views.end())
                        {
                            window->views.erase(item);
                        }
                    }
                }
            }

            viewRepository->remove(viewId);
        });
    }
}