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
        auto materialBindingRepository = _registry.get<IMaterialBindingRepository>(graphicalContext.graphicalSystem);
        auto windowService = _registry.get<services::IWindowService>(window.windowSystem);

        if(gpuDevice && meshBindingRepository && materialBindingRepository && windowService)
        {
            auto frame = Frame{
                Viewport{ view.viewport }
            };
            frame.clearColor = Vector4f{ 0.01f, 0.08f, 0.01f, 1.f };
            float aspectRatio = (float)window.width / (float)window.height;
            float halfWidth = 1.0f * aspectRatio;
            float halfHeight = 1.0f;
            frame.viewProjectionTransform = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);

            glm::mat4x4 rotateZ = glm::rotate(glm::identity<glm::mat4x4>(), glm::radians(10.f), glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4x4 rotateX = glm::rotate(glm::identity<glm::mat4x4>(), glm::radians(10.f), glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4x4 translate = glm::translate(glm::identity<glm::mat4x4>(), glm::vec3(0.f, 0.f, 0.f));
            glm::mat4x4 modelTransform = rotateX * rotateZ * translate;

            for(const auto& [meshId, meshBindingId] : graphicalContext.meshBindings)
            {
                auto* meshBinding = meshBindingRepository->get(meshBindingId);
                if(meshBinding)
                {
                    auto* material = materialBindingRepository->get(meshBinding->materialBindingId);

                    if(!frame.meshMap.contains(material))
                    {
                        frame.meshMap[material] = {};
                    }

                    frame.meshMap[material].emplace_back(MeshItem{ meshBinding, modelTransform });
                }
            }

            windowService->setCurrentWindow(window.id);
            gpuDevice->drawFrame(frame);
            windowService->swapBuffers(window);
        }
    }
}