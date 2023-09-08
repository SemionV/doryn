#include "baseTests/dependencies.h"

TEST_CASE("apply function", "[transformations]")
{
    fakeit::Mock<dory::MatrixCalculator> matrixCalculatorMock;
    std::shared_ptr<dory::MatrixCalculator> matrixCalculator(&matrixCalculatorMock.get(), [](dory::MatrixCalculator*){});
    
    auto transformationCalculator = std::make_shared<dory::TransformationCalculator>(matrixCalculator);
}