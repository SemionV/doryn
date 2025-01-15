#include <dory/core/registry.h>
#include "cameraService.h"
#include <dory/core/resources/scene/enttScene.h>
#include <dory/core/resources/scene/components.h>

namespace dory::game::logic
{
    glm::vec3 getMoveDirectionVector(MoveDirection direction, float distance)
    {
        glm::vec3 directionVector {};

        if(direction == MoveDirection::left)
        {
            directionVector = glm::vec3 { -distance, 0.f, 0.f };
        }
        else if(direction == MoveDirection::right)
        {
            directionVector = glm::vec3 { distance, 0.f, 0.f };
        }
        else if(direction == MoveDirection::up)
        {
            directionVector = glm::vec3 { 0.f, distance, 0.f };
        }
        if(direction == MoveDirection::down)
        {
            directionVector = glm::vec3 { 0.f, -distance, 0.f };
        }

        return directionVector;
    }

    float calculateAcceleration(float highVelocity, float lowVelocity, float accelerationDistance)
    {
        /*return ((highVelocity * highVelocity) - (lowVelocity * lowVelocity)) / (2 * accelerationDistance);*/

        return 0.f;
    }

    CameraService::CameraService(core::Registry& registry) : DependencyResolver(registry)
    {}

    void CameraService::moveCamera(core::resources::DataContext& dataContext, MoveDirection directionValue, const core::resources::entities::View& view)
    {
        auto sceneRepo = _registry.get<core::repositories::ISceneRepository>(view.sceneEcsType);
        auto sceneService = _registry.get<core::services::ISceneService>();
        if(sceneRepo && sceneService)
        {
            auto* scene = sceneRepo->get(view.sceneId);
            if(scene)
            {
                core::resources::scene::components::LinearMovement* linearMovement {};
                sceneService->getComponent(view.cameraId, *scene, &linearMovement);

                if(!linearMovement)
                {
                    float distance = 1.f;
                    glm::vec3 stepVector = getMoveDirectionVector(directionValue, distance);

                    float accelerationDistance = distance;
                    float highVelocity = 0.3f;
                    float startVelocity = highVelocity;
                    float lowVelocity = 0.01f;

                    if(linearMovement)
                    {
                        sceneService->removeComponent(view.cameraId, *scene, *linearMovement);
                    }

                    sceneService->addComponent(view.cameraId, *scene, core::resources::scene::components::LinearMovement {
                            true,
                            stepVector,
                            startVelocity,
                            highVelocity,
                            lowVelocity,
                            startVelocity,
                            accelerationDistance,
                            calculateAcceleration(highVelocity, startVelocity, accelerationDistance),
                            -1.f * calculateAcceleration(highVelocity, startVelocity, accelerationDistance)
                    });

                    /*dataContext.profiling.captureFrameStatistics = true;
                    dataContext.profiling.captureFrameBuffers = true;*/
                }
            }
        }
    }

    void CameraService::stopCamera(core::resources::DataContext& dataContext, const core::resources::entities::View& view)
    {
        auto sceneRepo = _registry.get<core::repositories::ISceneRepository>(view.sceneEcsType);
        auto sceneService = _registry.get<core::services::ISceneService>();
        if(sceneRepo && sceneService)
        {
            auto* scene = sceneRepo->get(view.sceneId);
            if(scene)
            {
                core::resources::scene::components::LinearMovement* linearMovement {};
                sceneService->getComponent(view.cameraId, *scene, &linearMovement);

                if(linearMovement)
                {
                    sceneService->removeComponent(view.cameraId, *scene, *linearMovement);
                    /*dataContext.profiling.captureFrameStatistics = false;
                    dataContext.profiling.captureFrameBuffers = false;*/
                }
            }
        }
    }
}