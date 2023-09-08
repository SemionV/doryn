#include "baseTests/dependencies.h"

using namespace fakeit;

#define multiplyM4P3Signature void(const dory::Matrix4x4* matrix, const dory::Point3d* vector, dory::Point3d* resultVector)

TEST_CASE("apply matrix transformation", "[transformations]")
{
    fakeit::Mock<dory::IMatrixCalculator> matrixCalculatorMock;
    std::shared_ptr<dory::IMatrixCalculator> matrixCalculator(&matrixCalculatorMock.get(), [](dory::IMatrixCalculator*){});

    std::shared_ptr<dory::Point3d> point = std::make_shared<dory::Point3d>();
    std::shared_ptr<dory::Point3d> resultPoint = std::make_shared<dory::Point3d>();

    std::shared_ptr<dory::Transformation3d> transformation = std::make_shared<dory::Transformation3d>();
    std::shared_ptr<dory::Matrix4x4> matrix = std::make_shared<dory::Matrix4x4>();
    transformation->matrix = matrix.get();

    Fake(OverloadedMethod(matrixCalculatorMock, multiply, multiplyM4P3Signature).Using(matrix.get(), point.get(), resultPoint.get())); 

    auto transformationCalculator = std::make_shared<dory::TransformationCalculator>(matrixCalculator);

    transformationCalculator->apply(transformation.get(), point.get(), resultPoint.get());

    Verify(OverloadedMethod(matrixCalculatorMock, multiply, multiplyM4P3Signature));
}