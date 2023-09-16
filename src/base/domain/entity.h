#pragma once

#include "base/dependencies.h"

namespace dory
{
    struct Entity
    {
        int id;
    };

    struct Camera: public Entity
    {
    };

    struct Window: public Entity
    {
    };

    struct Viewport
    {
        int x;
        int y;
        int width;
        int height;

        Viewport(int x, int y, int width, int height):
            x(x),
            y(y),
            width(width),
            height(height)
        {            
        }
    };

    struct View: public Entity
    {
        int windowId;
        int cameraId;
        Viewport viewport;

        View(int windowId, int cameraId, Viewport viewport):
            windowId(windowId),
            cameraId(cameraId),
            viewport(viewport)
        {}
    };
}