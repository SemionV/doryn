#include "baseTests/dependencies.h"

using Matrix4x4Entries = std::array<float, dory::Matrix4x4::size>;

TEST_CASE("get entry position 4x4", "[matricies]")
{
    auto position = dory::Matrix4x4::getEntryPositionByIndex(0);
    REQUIRE(position.x == 0);
    REQUIRE(position.y == 0);
    REQUIRE(dory::Matrix4x4::isDiagonalEntry(position) == true);

    position = dory::Matrix4x4::getEntryPositionByIndex(1);
    REQUIRE(position.x == 1);
    REQUIRE(position.y == 0);
    REQUIRE(dory::Matrix4x4::isDiagonalEntry(position) == false);

    position = dory::Matrix4x4::getEntryPositionByIndex(2);
    REQUIRE(position.x == 2);
    REQUIRE(position.y == 0);
    REQUIRE(dory::Matrix4x4::isDiagonalEntry(position) == false);

    position = dory::Matrix4x4::getEntryPositionByIndex(3);
    REQUIRE(position.x == 3);
    REQUIRE(position.y == 0);
    REQUIRE(dory::Matrix4x4::isDiagonalEntry(position) == false);

    position = dory::Matrix4x4::getEntryPositionByIndex(4);
    REQUIRE(position.x == 0);
    REQUIRE(position.y == 1);
    REQUIRE(dory::Matrix4x4::isDiagonalEntry(position) == false);

    position = dory::Matrix4x4::getEntryPositionByIndex(5);
    REQUIRE(position.x == 1);
    REQUIRE(position.y == 1);
    REQUIRE(dory::Matrix4x4::isDiagonalEntry(position) == true);

    position = dory::Matrix4x4::getEntryPositionByIndex(6);
    REQUIRE(position.x == 2);
    REQUIRE(position.y == 1);
    REQUIRE(dory::Matrix4x4::isDiagonalEntry(position) == false);

    position = dory::Matrix4x4::getEntryPositionByIndex(7);
    REQUIRE(position.x == 3);
    REQUIRE(position.y == 1);
    REQUIRE(dory::Matrix4x4::isDiagonalEntry(position) == false);

    position = dory::Matrix4x4::getEntryPositionByIndex(8);
    REQUIRE(position.x == 0);
    REQUIRE(position.y == 2);
    REQUIRE(dory::Matrix4x4::isDiagonalEntry(position) == false);

    position = dory::Matrix4x4::getEntryPositionByIndex(9);
    REQUIRE(position.x == 1);
    REQUIRE(position.y == 2);
    REQUIRE(dory::Matrix4x4::isDiagonalEntry(position) == false);

    position = dory::Matrix4x4::getEntryPositionByIndex(10);
    REQUIRE(position.x == 2);
    REQUIRE(position.y == 2);
    REQUIRE(dory::Matrix4x4::isDiagonalEntry(position) == true);

    position = dory::Matrix4x4::getEntryPositionByIndex(11);
    REQUIRE(position.x == 3);
    REQUIRE(position.y == 2);
    REQUIRE(dory::Matrix4x4::isDiagonalEntry(position) == false);

    position = dory::Matrix4x4::getEntryPositionByIndex(12);
    REQUIRE(position.x == 0);
    REQUIRE(position.y == 3);
    REQUIRE(dory::Matrix4x4::isDiagonalEntry(position) == false);

    position = dory::Matrix4x4::getEntryPositionByIndex(13);
    REQUIRE(position.x == 1);
    REQUIRE(position.y == 3);
    REQUIRE(dory::Matrix4x4::isDiagonalEntry(position) == false);

    position = dory::Matrix4x4::getEntryPositionByIndex(14);
    REQUIRE(position.x == 2);
    REQUIRE(position.y == 3);
    REQUIRE(dory::Matrix4x4::isDiagonalEntry(position) == false);

    position = dory::Matrix4x4::getEntryPositionByIndex(15);
    REQUIRE(position.x == 3);
    REQUIRE(position.y == 3);
    REQUIRE(dory::Matrix4x4::isDiagonalEntry(position) == true);
}

TEST_CASE( "setToIdentity 4x4", "[matricies]" ) 
{
    std::shared_ptr<dory::IMatrixCalculator> calculator = std::make_shared<dory::MatrixCalculator>();
    std::shared_ptr<dory::Matrix4x4> matrix = std::make_shared<dory::Matrix4x4>();

    const int matrixSize = 16;
    const int columnsSize = 4;
    const int rowsSize = 4;

    for(int i = 0; i < matrixSize; ++i)
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

    for(int i = 0; i < matrixSize; ++i)
    {
        dory::Point2d entryPosition = dory::Matrix4x4::getEntryPositionByIndex(i);
        auto entryValue = matrix->entries[i];

        if(dory::Matrix4x4::isDiagonalEntry(entryPosition))
        {
            REQUIRE(entryValue == 1);
        }
        else
        {
            REQUIRE(entryValue == 0);
        }
    }
}

TEST_CASE( "copy entries", "[matricies]" ) 
{
    std::shared_ptr<dory::IMatrixCalculator> calculator = std::make_shared<dory::MatrixCalculator>();
    std::shared_ptr<dory::Matrix4x4> matrix = std::make_shared<dory::Matrix4x4>();
    std::shared_ptr<dory::Matrix4x4> matrix2 = std::make_shared<dory::Matrix4x4>();

    const int matrixSize = 16;

    for(int i = 0; i < matrixSize; i++)
    {
        matrix->entries[i] = i;
    }

    calculator->copyEntriesTo(matrix.get(), matrix2.get());

    for(int i = 0; i < matrixSize; ++i)
    {
        REQUIRE(matrix->entries[i] == matrix2->entries[i]);
    }
}

TEST_CASE("multiply vector by matrix", "[matricies]")
{
    std::shared_ptr<dory::IMatrixCalculator> calculator = std::make_shared<dory::MatrixCalculator>();
    std::shared_ptr<dory::Matrix4x4> matrix = std::make_shared<dory::Matrix4x4>();

    /*
    |1 2 1 3|   |3|   |1*3 + 2*4 + 1*7 + 3*1|   |03+08+07+03|   |21|   |21/55|   |0.381818|
    |6 2 2 1|   |4|   |6*3 + 2*4 + 2*7 + 1*1|   |18+08+14+01|   |41|   |41/55|   |0.745454|
    |2 5 1 1| * |7| = |2*3 + 5*4 + 1*7 + 1*1| = |06+20+07+01| = |34| = |34/55| = |0.618181|
    |5 8 1 1|   |1|   |5*3 + 8*4 + 1*7 + 1*1|   |15+32+07+01|   |55|   
    */

    dory::Matrix4x4::setEntries(matrix.get(), Matrix4x4Entries {
            1, 2, 1, 3,
            6, 2, 2, 1,
            2, 5, 1, 1,
            5, 8, 1, 1
        }.data());

    std::shared_ptr<dory::Point3d> vector = std::make_shared<dory::Point3d>(3, 4, 7);
    std::shared_ptr<dory::Point3d> resultVector = std::make_shared<dory::Point3d>(0, 0, 0);

    calculator->multiply(matrix.get(), vector.get(), resultVector.get());

    REQUIRE(std::floor(resultVector->x * 100) == 38);
    REQUIRE(std::floor(resultVector->y * 100) == 74);
    REQUIRE(std::floor(resultVector->z * 100) == 61);
}

TEST_CASE("multiply matrix by matrix", "[matricies]")
{
    std::shared_ptr<dory::IMatrixCalculator> calculator = std::make_shared<dory::MatrixCalculator>();
    std::shared_ptr<dory::Matrix4x4> matrixLeft = std::make_shared<dory::Matrix4x4>();
    std::shared_ptr<dory::Matrix4x4> matrixRight = std::make_shared<dory::Matrix4x4>();
    std::shared_ptr<dory::Matrix4x4> resultMatrix = std::make_shared<dory::Matrix4x4>();

    dory::Matrix4x4::setEntries(matrixLeft.get(), Matrix4x4Entries {
            1, 2, 1, 3,
            6, 2, 2, 1,
            2, 5, 1, 1,
            5, 8, 1, 1
        }.data());

    dory::Matrix4x4::setEntries(matrixRight.get(), Matrix4x4Entries {
            1, 6, 2, 5,
            2, 2, 5, 8,
            1, 2, 1, 1,
            3, 1, 1, 1
        }.data());

    calculator->multiply(matrixLeft.get(), matrixRight.get(), resultMatrix.get());

    /*
    15 15 16 25
    15 45 25 49
    16 25 31 52
    25 49 52 91
    */

   std::shared_ptr<dory::Matrix4x4> matrixRequire = std::make_shared<dory::Matrix4x4>();
    dory::Matrix4x4::setEntries(matrixRequire.get(), Matrix4x4Entries {
            15, 15, 16, 25,
            15, 45, 25, 49,
            16, 25, 31, 52,
            25, 49, 52, 91
        }.data());

   REQUIRE(dory::Matrix4x4::isEqual(matrixRequire.get(), resultMatrix.get()));
}