#pragma once

#include "base/dependencies.h"

namespace dory
{
    struct Entity
    {
        int id;

        Entity(int id):
            id(id)
        {}
    };

    struct Camera: public Entity
    {
        Camera(int id):
            Entity(id)
        {}
    };

    struct Window
    {
        int id;

        Window(int id):
            id(id)
        {}
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

        View(int id, int windowId, int cameraId, Viewport viewport):
            Entity(id),
            windowId(windowId),
            cameraId(cameraId),
            viewport(viewport)
        {}
    };
}