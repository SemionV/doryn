#pragma once

#include "base/dependencies.h"
#include "point.h"
#include "matrix.h"

namespace dory
{
    template<typename TPoint, typename TMatrix>
    struct Transformation
    {
        Transformation<TPoint, TMatrix>* previous;
        TPoint* translation;
        TMatrix* matrix;
        std::function<void(const TPoint*, TPoint*)>* function;
    };

    struct Transformation3d: public Transformation<Point3d, Matrix4x4>
    {
    };
}