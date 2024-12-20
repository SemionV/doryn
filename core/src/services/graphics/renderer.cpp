#include <dory/core/registry.h>
#include <dory/core/services/graphics/renderer.h>
#include <dory/core/resources/objects/frame.h>
#include <dory/math/linearAlgebra.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

    glm::mat4x4 getTransformMatrix(const scene::components::Transform& transform)
    {
        auto matrix = glm::mat4x4{ 1 };
        matrix = glm::scale(matrix, transform.scale);
        matrix *= glm::toMat4(transform.rotation);
        matrix = glm::translate(matrix, transform.position);

        return matrix;
    }

    void Renderer::draw(DataContext& context, const Window& window, const GraphicalContext& graphicalContext, const View& view)
    {
        auto scene = view.scene;
        if(scene)
        {
            auto gpuDevice = _registry.get<IGpuDevice>(graphicalContext.graphicalSystem);
            auto meshBindingRepository = _registry.get<IMeshBindingRepository>(graphicalContext.graphicalSystem);
            auto materialBindingRepository = _registry.get<IMaterialBindingRepository>(graphicalContext.graphicalSystem);
            auto windowService = _registry.get<services::IWindowService>();
            auto sceneQueryService = _registry.get<services::ISceneQueryService>(scene->ecsType);

            if(gpuDevice && meshBindingRepository && materialBindingRepository && windowService && sceneQueryService)
            {
                auto frame = Frame{ Viewport{ view.viewport } };
                frame.clearColor = Vector4f{ 0.01f, 0.08f, 0.01f, 1.f };

                float aspectRatio = (float)window.width / (float)window.height;
                float halfWidth = 1.0f * aspectRatio;
                float halfHeight = 1.0f;
                frame.viewProjectionTransform = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);

                auto visibleObjects = sceneQueryService->getVisibleObjects(*scene);
                for(auto& object : visibleObjects)
                {
                    auto localTransform = getTransformMatrix(object.localTransform);
                    auto worldTransform = getTransformMatrix(object.worldTransform);
                    glm::mat4x4 modelTransform = worldTransform * localTransform;

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

                            frame.meshMap[material].emplace_back(MeshItem{ meshBinding, modelTransform });
                        }
                    }
                }

                windowService->setCurrentWindow(window);
                gpuDevice->drawFrame(frame);
                windowService->swapBuffers(window);
            }
        }
    }
}