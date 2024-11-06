#pragma once

#include "../entity.h"
#include <optional>

namespace dory::core::resources::entities
{
    struct Viewport
    {
        int x {};
        int y {};
        int width {};
        int height {};

        Viewport() = default;

        Viewport(int x, int y, int width, int height):
                x(x),
                y(y),
                width(width),
                height(height)
        {}
    };

    struct View: public Entity<IdType>
    {
        IdType windowId;
        IdType cameraId {};
        std::optional<Viewport> viewport {};

        View(IdType id, IdType windowId, IdType cameraId = nullId):
                Entity(id),
                windowId(windowId),
                cameraId(cameraId)
        {}
    };
}