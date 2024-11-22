#include <dory/core/registry.h>
#include <graphicalContextService.h>

namespace dory::renderer::opengl
{
    GraphicalContextService::GraphicalContextService(core::Registry &registry):
        _registry(registry)
    {}

    void GraphicalContextService::bindMesh(core::resources::assets::Mesh* mesh, core::resources::IdType sceneId)
    {
        auto viewService = _registry.get<core::services::IViewService>();
        auto windowService = _registry.get<core::services::IWindowService>();
        if(viewService && windowService)
        {
            auto windows = viewService->getWindowsByScene(sceneId);
            for(const auto window : windows)
            {
                auto graphicalContextRepository = _registry.get<core::repositories::IGraphicalContextRepository>(window->graphicalSystem);
                if(graphicalContextRepository)
                {
                    auto graphicalContext = graphicalContextRepository->get(window->graphicalContextId);
                    if(graphicalContext)
                    {
                        windowService->setCurrentWindow(window->id);
                        bindMesh(graphicalContext, mesh);
                    }
                }
            }
        }
    }

    core::resources::bindings::MeshBinding* GraphicalContextService::getMeshBinding(core::resources::IdType meshId, core::resources::IdType graphicalContextId)
    {
        return nullptr;
    }

    void GraphicalContextService::bindMesh(core::resources::entities::GraphicalContext* graphicalContext, core::resources::assets::Mesh* mesh)
    {
        auto meshBindingRepository = _registry.get<core::repositories::bindings::IMeshBindingRepository>(graphicalContext->graphicalSystem);
        auto gpuDriver = _registry.get<core::services::graphics::IGpuDriver>(graphicalContext->graphicalSystem);

        if(meshBindingRepository && gpuDriver)
        {
            core::resources::bindings::MeshBinding* meshBinding;

            if(graphicalContext->meshBindings.contains(mesh->id))
            {
                meshBinding = meshBindingRepository->get(graphicalContext->meshBindings[mesh->id]);
                gpuDriver->releaseMesh(meshBinding);
            }
            else
            {
                meshBinding = meshBindingRepository->insert(core::resources::bindings::MeshBinding{});
            }

            if(meshBinding)
            {
                graphicalContext->meshBindings[mesh->id] = meshBinding->id;
                gpuDriver->uploadMesh(mesh, meshBinding);
            }
        }
    }
}