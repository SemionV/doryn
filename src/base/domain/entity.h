#pragma once

#include "base/dependencies.h"

namespace dory
{
    template<typename TId>
    struct Entity
    {
        TId id;
    };

    struct Camera: public Entity<int>
    {
    };

    struct Window: public Entity<int>
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

    struct View: public Entity<int>
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

    struct ComponentReference: Entity<int>
    {
        std::shared_ptr<void> component;

        public:
            ComponentReference(std::shared_ptr<void> component):
                component(component)
            {}
    };
}