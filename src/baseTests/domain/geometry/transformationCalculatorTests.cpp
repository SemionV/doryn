#include "baseTests/dependencies.h"
#include "base/base.h"

using namespace fakeit;
using namespace dory::domain;

#define multiplyM4P3Signature void(const geometry::Matrix4x4* matrix, const geometry::Point3d* vector, geometry::Point3d* resultVector)

TEST_CASE("apply matrix transformation", "[transformations]")
{
    fakeit::Mock<services::IMatrixCalculatorService> matrixCalculatorMock;
    std::shared_ptr<services::IMatrixCalculatorService> matrixCalculator(&matrixCalculatorMock.get(), [](services::IMatrixCalculatorService*) {});

    std::shared_ptr<geometry::Point3d> point = std::make_shared<geometry::Point3d>();
    std::shared_ptr<geometry::Point3d> resultPoint = std::make_shared<geometry::Point3d>();

    std::shared_ptr<geometry::Transformation3d> transformation = std::make_shared<geometry::Transformation3d>();
    std::shared_ptr<geometry::Matrix4x4> matrix = std::make_shared<geometry::Matrix4x4>();
    transformation->matrix = matrix.get();

    Fake(OverloadedMethod(matrixCalculatorMock, multiply, multiplyM4P3Signature).Using(matrix.get(), point.get(), resultPoint.get())); 

    auto transformationCalculator = std::make_shared<services::TransformationCalculatorService>(matrixCalculator);

    transformationCalculator->apply(transformation.get(), point.get(), resultPoint.get());

    Verify(OverloadedMethod(matrixCalculatorMock, multiply, multiplyM4P3Signature));
}

TEST_CASE("apply function transformation", "[transformations]")
{
    std::shared_ptr<geometry::Point3d> point = std::make_shared<geometry::Point3d>(1, 1, 1);
    std::shared_ptr<geometry::Point3d> resultPoint = std::make_shared<geometry::Point3d>();

    std::shared_ptr<geometry::Transformation3d> transformation = std::make_shared<geometry::Transformation3d>();
    auto function = std::function<void(const geometry::Point3d* point, geometry::Point3d* resultPoint)>([](const geometry::Point3d* point, geometry::Point3d* resultPoint)
    {
        resultPoint->x = point->x + 2;
        resultPoint->y = point->y + 2;
        resultPoint->z = point->z + 2;
    });
    transformation->function = &function;

    auto transformationCalculator = std::make_shared<services::TransformationCalculatorService>(nullptr);

    transformationCalculator->apply(transformation.get(), point.get(), resultPoint.get());

    REQUIRE(resultPoint->x == 3);
    REQUIRE(resultPoint->z == 3);
    REQUIRE(resultPoint->y == 3);
}

TEST_CASE("apply translation transformation", "[transformations]")
{
    std::shared_ptr<geometry::Point3d> point = std::make_shared<geometry::Point3d>(1, 1, 1);
    std::shared_ptr<geometry::Point3d> resultPoint = std::make_shared<geometry::Point3d>();

    std::shared_ptr<geometry::Transformation3d> transformation = std::make_shared<geometry::Transformation3d>();
    auto vector = std::make_shared<geometry::Point3d>(2, 2, 2);
    transformation->translation = vector.get();

    auto transformationCalculator = std::make_shared<services::TransformationCalculatorService>(nullptr);

    transformationCalculator->apply(transformation.get(), point.get(), resultPoint.get());

    REQUIRE(resultPoint->x == 3);
    REQUIRE(resultPoint->z == 3);
    REQUIRE(resultPoint->y == 3);
}

TEST_CASE("apply chain of transformations", "[transformations]")
{
    auto stack = std::vector<int>();

    std::shared_ptr<geometry::Transformation3d> transformationRight = std::make_shared<geometry::Transformation3d>();
    auto functionRight = std::function<void(const geometry::Point3d* point, geometry::Point3d* resultPoint)>([&stack](const geometry::Point3d* point, geometry::Point3d* resultPoint)
    {
        stack.emplace_back(1);
    });
    transformationRight->function = &functionRight;

    std::shared_ptr<geometry::Transformation3d> transformationLeft = std::make_shared<geometry::Transformation3d>();
    auto functionLeft = std::function<void(const geometry::Point3d* point, geometry::Point3d* resultPoint)>([&stack](const geometry::Point3d* point, geometry::Point3d* resultPoint)
    {
        stack.emplace_back(2);
    });
    transformationLeft->function = &functionLeft;
    transformationLeft->previous = transformationRight.get();

    auto transformationCalculator = std::make_shared<services::TransformationCalculatorService>(nullptr);

    transformationCalculator->apply(transformationLeft.get(), nullptr, nullptr);

    REQUIRE(stack.size() == 2);
    REQUIRE(stack[0] == 1); //right transformation has to be applied first
    REQUIRE(stack[1] == 2); //left transformation has to be applied second
}