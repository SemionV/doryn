#pragma once

#include "transformation.h"
#include "matrixCalculator.h"

namespace dory
{
    class TransformationCalculator
    {
        private:
            std::shared_ptr<MatrixCalculator> matrixCalculator;

        public:
            TransformationCalculator(std::shared_ptr<MatrixCalculator> matrixCalculator);

            void apply(Transformation<Point3d, Matrix4x4>* transformation, const Point3d* point, Point3d* resultPoint);
    };
}