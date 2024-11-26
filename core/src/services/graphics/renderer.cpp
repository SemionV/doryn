#include <dory/core/registry.h>
#include <dory/core/services/graphics/renderer.h>
#include <dory/core/resources/objects/frame.h>
#include <dory/math/linearAlgebra.h>

namespace dory::core::services::graphics
{
    using namespace resources;
    using namespace resources::bindings;
    using namespace resources::objects;
    using namespace resources::entities;
    using namespace services;
    using namespace devices;
    using namespace math;
    using namespace repositories;
    using namespace repositories::bindings;

    Renderer::Renderer(Registry& registry) : DependencyResolver(registry)
    {}

    void Renderer::draw(DataContext& context, const View& view)
    {}

    void Renderer::draw(DataContext& context, const Window& window, const GraphicalContext& graphicalContext, const View& view)
    {
        auto gpuDevice = _registry.get<IGpuDevice>(graphicalContext.graphicalSystem);
        auto meshBindingRepository = _registry.get<IMeshBindingRepository>(graphicalContext.graphicalSystem);

        if(gpuDevice && meshBindingRepository)
        {
            auto frame = Frame{};
            frame.clearColor = Vector4f{ 0.01f, 0.08f, 0.01f, 1.f };

            for(const auto& [meshId, meshBindingId] : graphicalContext.meshBindings)
            {
                auto* meshBinding = meshBindingRepository->get(meshBindingId);
                if(meshBinding)
                {
                    frame.meshes.emplace_back(meshBinding);
                }
            }

            gpuDevice->drawFrame(frame);
        }
    }
}