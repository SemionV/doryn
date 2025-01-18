#include <dory/core/registry.h>
#include <dory/core/services/viewService.h>

namespace dory::core::services
{
    ViewService::ViewService(Registry& registry) : DependencyResolver(registry)
    {}

    void ViewService::updateViews(resources::scene::SceneViewStateSet& viewStates, float alpha, resources::profiling::Profiling& profiling)
    {
        auto windowRepository = _registry.get<repositories::IWindowRepository>();
        auto graphicalContextRepository = _registry.get<repositories::IGraphicalContextRepository>();
        auto renderer = _registry.get<graphics::IRenderer>();

        if(windowRepository && graphicalContextRepository && renderer)
        {
            for(const auto& [viewId, viewState] : viewStates)
            {
                if(const auto window = windowRepository->get(viewState.view.windowId))
                {
                    if(const auto graphicalContext = graphicalContextRepository->get(window->graphicalContextId))
                    {
                        renderer->draw(viewState, alpha, *window, *graphicalContext, profiling);
                    }
                }
            }
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

    void ViewService::updateViewsState(resources::scene::SceneViewStateSet& states)
    {
        auto viewRepository = _registry.get<repositories::IViewRepository>();

        if(viewRepository)
        {
            std::vector<resources::IdType> deleteList;

            for(const auto& [viewId, state] : states)
            {
                if(!viewRepository->get(viewId))
                {
                    deleteList.emplace_back(viewId);
                }
            }

            for(auto viewId : deleteList)
            {
                states.erase(viewId);
            }

            viewRepository->each([&](resources::entities::View& view) {
                auto sceneQueryService = _registry.get<services::ISceneQueryService>(view.sceneEcsType);
                auto sceneRepo = _registry.get<repositories::ISceneRepository>(view.sceneEcsType);
                resources::scene::SceneViewState* viewState {};

                auto it = states.find(view.id);
                if(it != states.end())
                {
                    viewState = &(it->second);
                }
                else
                {
                    viewState = &states.emplace(view.id, resources::scene::SceneViewState{}).first->second;
                }

                if(viewState)
                {
                    if(sceneQueryService && sceneRepo)
                    {
                        auto scene = sceneRepo->get(view.sceneId);

                        if(viewState->view.sceneId == view.sceneId)
                        {
                            viewState->previous = viewState->current;
                        }
                        else //In this case the view's scene was changed, and we have to start over and set the previous state to default value
                        {
                            viewState->previous = {};
                        }

                        viewState->current = sceneQueryService->getSceneState(*scene);
                    }

                    viewState->view = view;
                }
            });
        }
    }
}