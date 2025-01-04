#include <dory/core/registry.h>
#include "cameraService.h"
#include <dory/core/resources/scene/enttScene.h>
#include <dory/core/resources/scene/components.h>

namespace dory::game::logic
{
    CameraService::CameraService(core::Registry& registry) : DependencyResolver(registry)
    {}

    void CameraService::moveCamera(MoveDirection direction, const core::resources::entities::View& view)
    {
        auto sceneRepo = _registry.get<core::repositories::ISceneRepository>(view.sceneEcsType);
        auto sceneService = _registry.get<core::services::ISceneService>();
        if(sceneRepo && sceneService)
        {
            auto* scene = sceneRepo->get(view.sceneId);
            if(scene)
            {
                auto* enttScene = (core::resources::scene::EnttScene*)scene;
                auto& registry = enttScene->registry;

                auto it = enttScene->idMap.find(view.cameraId);
                if(it != enttScene->idMap.end())
                {
                    auto cameraEntity = it->second;
                    auto* linearMovement = registry.try_get<core::resources::scene::components::LinearMovement>(cameraEntity);

                    glm::vec3 directionVector {};
                    float distance = 0.5f;

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

                    if(linearMovement)
                    {

                    }
                    else
                    {
                        float accelerationDistance = 0.3f;
                        float startVelocity = 0.0f;
                        float highVelocity = 0.7f;
                        float lowVelocity = 0.01f;
                        sceneService->addComponent(view.cameraId, *scene, core::resources::scene::components::LinearMovement {
                                directionVector,
                                startVelocity,
                                highVelocity,
                                lowVelocity,
                                0.f,
                                accelerationDistance,
                                ((highVelocity * highVelocity) - (startVelocity * startVelocity)) / (2 * accelerationDistance),
                                -((highVelocity * highVelocity) - (lowVelocity * lowVelocity)) / (2 * accelerationDistance)
                        });
                    }
                }
            }
        }
    }
}