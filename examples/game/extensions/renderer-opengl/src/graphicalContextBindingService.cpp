#include <dory/core/registry.h>
#include <graphicalContextBindingService.h>

namespace dory::renderer::opengl
{
    GraphicalContextBindingsService::GraphicalContextBindingsService(core::Registry &registry):
        _registry(registry)
    {}

    void GraphicalContextBindingsService::bindMesh(core::resources::assets::Mesh* mesh, core::resources::IdType sceneId)
    {
        auto viewService = _registry.get<core::services::IViewService>();
        auto windowService = _registry.get<core::services::IWindowService>();
        if(viewService && windowService)
        {
            auto windows = viewService->getWindowsByScene(sceneId);
            for(const auto window : windows)
            {
                auto graphicalContextBindingsRepository = _registry.get<core::repositories::IGraphicalContextBindingsRepository>(window->graphicalSystem);
                if(graphicalContextBindingsRepository)
                {
                    auto graphicalContextBindings = graphicalContextBindingsRepository->get(window->graphicalContextBindingsId);
                    if(graphicalContextBindings)
                    {
                        windowService->setCurrentWindow(*window);
                        bindMesh(graphicalContextBindings, mesh);
                    }
                }
            }
        }
    }

    core::resources::bindings::MeshBinding* GraphicalContextBindingsService::getMeshBinding(core::resources::IdType meshId, core::resources::IdType graphicalContextBindingsId)
    {
        return nullptr;
    }

    void GraphicalContextBindingsService::bindMesh(core::resources::entities::Window* window, core::resources::entities::GraphicalContextBindings* graphicalContextBindings, core::resources::assets::Mesh* mesh)
    {
        auto meshBindingRepository = _registry.get<core::repositories::bindings::IMeshBindingRepository>(window->graphicalSystem);

        if(meshBindingRepository)
        {
            if(graphicalContextBindings->meshBindings.contains(mesh->id))
            {
                auto meshBinding = meshBindingRepository->get(graphicalContextBindings->meshBindings[mesh->id]);

                //TODO: call GpuClient to release binding
                //TODO: call GpuClient to bind mesh to the context
            }
            else
            {
                auto meshBinding = meshBindingRepository->insert(core::resources::bindings::MeshBinding{});
                if(meshBinding)
                {
                    graphicalContextBindings->meshBindings[mesh->id] = meshBinding->id;

                    //TODO: call GpuClient to bind mesh to the context
                }
            }
        }
    }
}