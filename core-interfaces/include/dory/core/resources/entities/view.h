#pragma once

#include "../entity.h"
#include "../ecsType.h"
#include "dory/core/resources/scene/scene.h"
#include <atomic>

namespace dory::core::resources::entities
{
    struct Viewport
    {
        unsigned int x {};
        unsigned int y {};
        unsigned int width {};
        unsigned int height {};

        Viewport() = default;

        Viewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height):
                x(x),
                y(y),
                width(width),
                height(height)
        {}
    };

    struct View: public Entity<IdType>
    {
    public:
        IdType windowId {};
        IdType cameraId {};
        resources::IdType sceneId {};
        resources::EcsType sceneEcsType {};
        Viewport viewport {};
        glm::mat4x4 projection {};

        View() = default;

        View(IdType id, IdType windowId, IdType cameraId = nullId):
                Entity(id),
                windowId(windowId),
                cameraId(cameraId)
        {}
    };
}