#include <dory/core/registry.h>
#include <dory/core/services/viewService.h>

namespace dory::core::services
{
    ViewService::ViewService(Registry& registry) : DependencyResolver(registry)
    {}

    void ViewService::updateViews(resources::scene::SceneViewStateSet& viewStates)
    {
        auto windowRepository = _registry.get<repositories::IWindowRepository>();
        auto graphicalContextRepository = _registry.get<repositories::IGraphicalContextRepository>();
        auto renderer = _registry.get<graphics::IRenderer>();

        if(windowRepository && graphicalContextRepository && renderer)
        {
            for(const auto& [viewId, viewState] : viewStates.map)
            {
                auto window = windowRepository->get(viewState.view.windowId);
                if(window)
                {
                    auto graphicalContext = graphicalContextRepository->get(window->graphicalContextId);
                    if(graphicalContext)
                    {
                        renderer->draw(viewState, viewStates.alpha, *window, *graphicalContext);
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

    void ViewService::updateViewsState(resources::scene::SceneViewStateSet& states, float alpha)
    {
        auto viewRepository = _registry.get<repositories::IViewRepository>();

        states.alpha = alpha;

        if(viewRepository)
        {
            std::vector<resources::IdType> deleteList;

            for(const auto& [viewId, state] : states.map)
            {
                if(!viewRepository->get(viewId))
                {
                    deleteList.emplace_back(viewId);
                }
            }

            for(auto viewId : deleteList)
            {
                states.map.erase(viewId);
            }

            viewRepository->each([&](resources::entities::View& view) {
                auto sceneQueryService = _registry.get<services::ISceneQueryService>(view.sceneEcsType);
                auto sceneRepo = _registry.get<repositories::ISceneRepository>(view.sceneEcsType);
                resources::scene::SceneViewState* viewState {};

                auto it = states.map.find(view.id);
                if(it != states.map.end())
                {
                    viewState = &(it->second);
                }
                else
                {
                    viewState = &(states.map.emplace(view.id, resources::scene::SceneViewState{}).first->second);
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
                        else //In this case the view's scene was changed, and we have to start over from the scratch and set the previous state to default value
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