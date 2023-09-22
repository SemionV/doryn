#pragma once

#include "base/domain/geometry/transformation.h"
#include "matrixCalculatorService.h"

namespace dory::domain::services
{
    class TransformationCalculatorService
    {
        private:
            std::shared_ptr<IMatrixCalculatorService> matrixCalculator;

        public:
            TransformationCalculatorService(std::shared_ptr<IMatrixCalculatorService> matrixCalculator);

            void apply(geometry::Transformation<geometry::Point3d, geometry::Matrix4x4>* transformation, const geometry::Point3d* point, geometry::Point3d* resultPoint);
    };
}