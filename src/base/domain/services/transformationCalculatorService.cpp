#include "base/dependencies.h"
#include "transformationCalculatorService.h"

namespace dory::domain::services
{
    TransformationCalculatorService::TransformationCalculatorService(std::shared_ptr<IMatrixCalculatorService> matrixCalculator):
        matrixCalculator(matrixCalculator)
    {
    }

    void TransformationCalculatorService::apply(geometry::Transformation<geometry::Point3d, geometry::Matrix4x4>* transformation, const geometry::Point3d* point, geometry::Point3d* resultPoint)
    {
        const geometry::Point3d* inputPoint = point;
        geometry::Point3d previousTransformationResult(0, 0, 0);

        if(transformation->previous)
        {
            apply(transformation->previous, inputPoint, &previousTransformationResult);
            inputPoint = &previousTransformationResult;
        }

        if(transformation->function)
        {
            transformation->function->operator()(inputPoint, resultPoint);
        }
        else if(transformation->matrix)
        {
            matrixCalculator->multiply(transformation->matrix, inputPoint, resultPoint);
        }
        else if(transformation->translation)
        {
            auto translation = transformation->translation;

            resultPoint->x = inputPoint->x + translation->x;
            resultPoint->y = inputPoint->y + translation->y;
            resultPoint->z = inputPoint->z + translation->z;
        }
    }
}