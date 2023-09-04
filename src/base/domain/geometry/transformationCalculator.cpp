#include "base/dependencies.h"
#include "transformationCalculator.h"

namespace dory
{
    TransformationCalculator::TransformationCalculator(std::shared_ptr<MatrixCalculator> matrixCalculator):
        matrixCalculator(matrixCalculator)
    {
    }

    void TransformationCalculator::apply(Transformation<Point3d, Matrix4x4>* transformation, const Point3d* point, Point3d* resultPoint)
    {
        const Point3d* inputPoint = point;
        Point3d previousTransformationResult(0, 0, 0);

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