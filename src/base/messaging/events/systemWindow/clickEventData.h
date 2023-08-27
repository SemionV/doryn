#pragma once

#include "base/dependencies.h"
#include "base/devices/window.h"

namespace dory
{
    struct MouseClickEventData
    {
        public:
            const int x;
            const int y;
            const std::shared_ptr<Window> window;

        public:
            MouseClickEventData(std::shared_ptr<Window> window, int x, int y):
                window(window),
                x(x),
                y(y)
            {                
            }
    };
}