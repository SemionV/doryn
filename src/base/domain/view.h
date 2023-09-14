#pragma once

#include "base/dependencies.h"
#include "window.h"
#include "viewport.h"
#include "camera.h"

namespace dory
{
    template<class TWindow>
    struct View
    {
        std::shared_ptr<TWindow> window;
        std::shared_ptr<Viewport> viewport;
        std::shared_ptr<Camera> camera;

        View(std::shared_ptr<TWindow> window, std::shared_ptr<Viewport> viewport, std::shared_ptr<Camera> camera):
            window(window),
            viewport(viewport),
            camera(camera)
        {            
        }
    };

    struct Entity
    {
        int id;

        Entity(int id):
            id(id)
        {}
    };

    struct CameraNi: public Entity
    {
        CameraNi(int id):
            Entity(id)
        {}
    };

    struct ViewportNi
    {
        int x;
        int y;
        int width;
        int height;

        ViewportNi(int x, int y, int width, int height):
            x(x),
            y(y),
            width(width),
            height(height)
        {            
        }
    };

    struct ViewNi: public Entity
    {
        int windowId;
        int cameraId;
        ViewportNi viewport;

        ViewNi(int id, int windowId, int cameraId, ViewportNi viewport):
            Entity(id),
            windowId(windowId),
            cameraId(cameraId),
            viewport(viewport)
        {}
    };
}