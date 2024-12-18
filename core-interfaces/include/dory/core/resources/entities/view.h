#pragma once

#include "../entity.h"

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
        IdType windowId {};
        IdType cameraId {};
        IdType sceneId {};
        Viewport viewport {};

        View() = default;

        View(IdType id, IdType windowId, IdType cameraId = nullId):
                Entity(id),
                windowId(windowId),
                cameraId(cameraId)
        {}
    };
}