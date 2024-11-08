#include <dory/core/registry.h>
#include <graphicalSystem.h>
#include <dory/core/repositories/iViewRepository.h>
#include <dory/core/services/iWindowService.h>

namespace dory::renderer::opengl
{
    GraphicalSystem::GraphicalSystem(core::Registry& registry):
        _registry(registry)
    {}

    void GraphicalSystem::render(core::resources::DataContext& context, const core::resources::entities::Window& window)
    {
        _registry.get<core::repositories::IViewRepository>([this, &context, &window](core::repositories::IViewRepository* viewRepository) {
            auto viewIds = viewRepository->getWindowViews(window.id);
            auto windowService = _registry.get<core::services::IWindowService>(window.windowSystem);
            auto renderer = _registry.get<core::services::IRenderer>(window.graphicalSystem);
            for(const auto& viewId : viewIds)
            {
                auto view = viewRepository->get(viewId);

                if(view && windowService && renderer)
                {
                    windowService->setCurrentWindow(window);
                    renderer->draw(context, *view);
                    windowService->swapBuffers(window);
                }
            }
        });
    }
}