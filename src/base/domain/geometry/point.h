#pragma once

namespace dory
{
    struct Point2d
    {
        float x;
        float y;

        Point2d(float x, float y):
            x(x), 
            y(y)
        {            
        }
    };

    struct Point3d: public Point2d
    {
        float z;

        Point3d(float x, float y):
            Point2d(x, y),
            z(z)
        {            
        }
    };
}