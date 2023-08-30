#pragma once

#include "base/dependencies.h"
#include "point.h"
#include "matrix.h"

namespace dory
{
    template<class TPoint>
    class Transformation
    {
        public:
            virtual void apply(const TPoint& point, TPoint& resultPoint) = 0;
            
            void operator()(const TPoint& point, TPoint& resultPoint)
            {
                apply(point, resultPoint);
            }
    };

    class IdentityTransformation3d: public Transformation<Point3d>
    {
        public:
            void apply(const Point3d& point, Point3d& resultPoint) override
            {
                resultPoint.x = point.x;
                resultPoint.y = point.y;
                resultPoint.z = point.z;
            }
    };

    class Translation3d: public Transformation<Point3d>
    {
        private:
            Point3d translationVector;

        public:
            Translation3d(Point3d&& translationVector):
                translationVector(std::forward<Point3d>(translationVector))
            {
            }

            void apply(const Point3d& point, Point3d& resultPoint) override
            {
                resultPoint.x = point.x + translationVector.x;
                resultPoint.y = point.y + translationVector.y;
                resultPoint.z = point.z + translationVector.z;
            }
    };
}