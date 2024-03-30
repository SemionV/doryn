#pragma once

#include <utility>

#include "base/dependencies.h"
#include "point.h"
#include "matrix.h"

namespace dory::domain::geometry
{
    template<typename TPreviousTransformation>
    struct Transformation
    {
        TPreviousTransformation previous;
    };

    template<>
    struct Transformation<void>
    {
    };

    template<typename TMatrix, typename TPreviousTransformation = void>
    struct MatrixTransformation: public Transformation<TPreviousTransformation>
    {
        TMatrix matrix;
    };
}