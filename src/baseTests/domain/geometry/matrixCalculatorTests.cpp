#include "baseTests/dependencies.h"

TEST_CASE( "setToIdentity 4x4", "[matricies]" ) 
{
    std::shared_ptr<dory::IMatrixCalculator> calculator = std::make_shared<dory::MatrixCalculator>();
    std::shared_ptr<dory::Matrix4x4> matrix = std::make_shared<dory::Matrix4x4>();

    const int matrixSize = 16;
    const int columnsSize = 4;
    const int rowsSize = 4;

    for(int i = 0; i < matrixSize; i++)
    {
        matrix->entries[i] = 0;
    }

    calculator->setToIdentity(matrix.get());

    /*
    0 1 2 3
    --------
    1 0 0 0 | 0
    0 1 0 0 | 1
    0 0 1 0 | 2
    0 0 0 1 | 3

    0 5 10 15
    */

    for(int i = 0; i < matrixSize; i++)
    {
        dory::Point2d entryPosition = dory::MatrixCalculator::getMatrix4x4PositionByIndex(i);
        auto entryValue = matrix->entries[i];

        if(dory::MatrixCalculator::isMatrix4x4DiagonalEntry(entryPosition))
        {
            REQUIRE(entryValue == 1);
        }
        else
        {
            REQUIRE(entryValue == 0);
        }
    }
}