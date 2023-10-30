#pragma once

namespace dory::domain::geometry
{
    struct Point2d
    {
        float x {};
        float y {};

        Point2d(float x, float y):
            x(x), 
            y(y)
        {            
        }

        Point2d():
            x(0), 
            y(0)
        {            
        }
    };

    struct Point3d: public Point2d
    {
        float z {};

        Point3d(float x, float y, float z):
            Point2d(x, y),
            z(z)
        {            
        }

        Point3d():
            z(0)
        {            
        }
    };
}