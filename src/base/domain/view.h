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
}