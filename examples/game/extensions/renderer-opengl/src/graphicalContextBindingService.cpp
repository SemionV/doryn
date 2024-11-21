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
                        bindMesh(window, graphicalContextBindings, mesh);
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
        auto gpuDriver = _registry.get<core::services::graphics::IGpuDriver>(window->graphicalSystem);

        if(meshBindingRepository && gpuDriver)
        {
            core::resources::bindings::MeshBinding* meshBinding;

            if(graphicalContextBindings->meshBindings.contains(mesh->id))
            {
                meshBinding = meshBindingRepository->get(graphicalContextBindings->meshBindings[mesh->id]);
                gpuDriver->releaseMesh(meshBinding);
            }
            else
            {
                meshBinding = meshBindingRepository->insert(core::resources::bindings::MeshBinding{});
            }

            if(meshBinding)
            {
                graphicalContextBindings->meshBindings[mesh->id] = meshBinding->id;
                gpuDriver->uploadMesh(mesh, meshBinding);
            }
        }
    }
}