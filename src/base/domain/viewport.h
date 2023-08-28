#pragma once

namespace dory
{
    struct Viewport
    {
        const int x;
        const int y;
        const int width;
        const int height;

        Viewport(int x, int y, int width, int height):
            x(x),
            y(y),
            width(width),
            height(height)
        {            
        }
    };
}