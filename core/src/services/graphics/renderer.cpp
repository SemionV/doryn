#include <dory/core/registry.h>
#include <dory/core/services/graphics/renderer.h>
#include <dory/core/resources/objects/frame.h>
#include <dory/math/linearAlgebra.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace dory::core::services::graphics
{
    using namespace resources;
    using namespace resources::bindings;
    using namespace resources::objects;
    using namespace resources::entities;
    using namespace resources::scene;
    using namespace services;
    using namespace devices;
    using namespace math;
    using namespace repositories;
    using namespace repositories::bindings;

    glm::mat4x4 getTransformMatrix(const objects::Transform& transform)
    {
        auto matrix = glm::mat4x4{ 1 };
        matrix = glm::scale(matrix, transform.scale);
        matrix = glm::toMat4(transform.rotation) * matrix;
        matrix = glm::translate(glm::mat4x4{ 1 }, transform.position) * matrix;

        return matrix;
    }

    Renderer::Renderer(Registry& registry) : DependencyResolver(registry)
    {}

    void Renderer::draw(const resources::scene::SceneViewState& viewState,
                        float alpha,
                        const resources::entities::Window& window,
                        const resources::entities::GraphicalContext& graphicalContext,
                        resources::DataContext& dataContext)
    {
        auto& view = viewState.view;

        auto gpuDevice = _registry.get<IGpuDevice>(graphicalContext.graphicalSystem);
        auto meshBindingRepository = _registry.get<IMeshBindingRepository>(graphicalContext.graphicalSystem);
        auto materialBindingRepository = _registry.get<IMaterialBindingRepository>(graphicalContext.graphicalSystem);
        auto windowService = _registry.get<services::IWindowService>();

        if(gpuDevice && meshBindingRepository && materialBindingRepository && windowService)
        {
            auto frame = Frame{ Viewport{ view.viewport } };
            frame.clearColor = Vector4f{ 0.01f, 0.08f, 0.01f, 1.f };
            frame.viewProjectionTransform = view.projection;

            for(const auto& [objectId, object] : viewState.current.objects)
            {
                if(graphicalContext.meshBindings.contains(object.meshId))
                {
                    IdType bindingId = graphicalContext.meshBindings.at(object.meshId);
                    auto* meshBinding = meshBindingRepository->get(bindingId);
                    if(meshBinding)
                    {
                        auto* material = materialBindingRepository->get(meshBinding->materialBindingId);

                        if(!frame.meshMap.contains(material))
                        {
                            frame.meshMap[material] = {};
                        }

                        objects::Transform transform {};

                        auto prevObjectIt = viewState.previous.objects.find(objectId);
                        if(prevObjectIt != viewState.previous.objects.end())
                        {
                            auto& prevTransform = prevObjectIt->second.transform;

                            transform.scale = glm::mix(prevTransform.scale, object.transform.scale, alpha);
                            transform.rotation = glm::slerp(prevTransform.rotation, object.transform.rotation, alpha);
                            transform.position = glm::mix(prevTransform.position, object.transform.position, alpha);

                            dataContext.currentFrame->alpha = alpha;
                        }
                        else
                        {
                            transform.scale = object.transform.scale;
                            transform.rotation = object.transform.rotation;
                            transform.position = object.transform.position;
                        }

                        frame.meshMap[material].emplace_back(MeshItem{ meshBinding, getTransformMatrix(transform) });
                    }
                }
            }

            windowService->setCurrentWindow(window);
            gpuDevice->drawFrame(frame, dataContext);
            windowService->swapBuffers(window);
        }
    }
}