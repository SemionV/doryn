#include "baseTests/dependencies.h"

using namespace dory::domain;

TEST_CASE("get entry position 4x4", "[matricies]")
{
    auto position = geometry::Matrix4x4::getEntryPositionByIndex(0);
    REQUIRE(position.column == 0);
    REQUIRE(position.row    == 0);
    REQUIRE(geometry::Matrix4x4::isDiagonalEntry(position) == true);

    position = geometry::Matrix4x4::getEntryPositionByIndex(14);
    REQUIRE(position.column == 2);
    REQUIRE(position.row    == 3);
    REQUIRE(geometry::Matrix4x4::isDiagonalEntry(position) == false);

    position = geometry::Matrix4x4::getEntryPositionByIndex(15);
    REQUIRE(position.column == 3);
    REQUIRE(position.row    == 3);
    REQUIRE(geometry::Matrix4x4::isDiagonalEntry(position) == true);
}

TEST_CASE( "setToIdentity 4x4", "[matricies]" ) 
{
    std::shared_ptr<services::IMatrixCalculatorService> calculator = std::make_shared<services::MatrixCalculatorService>();
    std::shared_ptr<geometry::Matrix4x4> matrix = std::make_shared<geometry::Matrix4x4>();

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
        auto entryPosition = geometry::Matrix4x4::getEntryPositionByIndex(i);
        auto entryValue = matrix->entries[i];

        if(geometry::Matrix4x4::isDiagonalEntry(entryPosition))
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
    std::shared_ptr<services::IMatrixCalculatorService> calculator = std::make_shared<services::MatrixCalculatorService>();
    std::shared_ptr<geometry::Matrix4x4> matrix = std::make_shared<geometry::Matrix4x4>();
    std::shared_ptr<geometry::Matrix4x4> matrix2 = std::make_shared<geometry::Matrix4x4>();

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
    std::shared_ptr<services::IMatrixCalculatorService> calculator = std::make_shared<services::MatrixCalculatorService>();
    std::shared_ptr<geometry::Matrix4x4> matrix = std::make_shared<geometry::Matrix4x4>(geometry::Matrix4x4::EntriesArray {
        1, 2, 1, 3,
        6, 2, 2, 1,
        2, 5, 1, 1,
        5, 8, 1, 1
    });

    /*
    |1 2 1 3|   |3|   |1*3 + 2*4 + 1*7 + 3*1|   |03+08+07+03|   |21|   |21/55|   |0.381818|
    |6 2 2 1|   |4|   |6*3 + 2*4 + 2*7 + 1*1|   |18+08+14+01|   |41|   |41/55|   |0.745454|
    |2 5 1 1| * |7| = |2*3 + 5*4 + 1*7 + 1*1| = |06+20+07+01| = |34| = |34/55| = |0.618181|
    |5 8 1 1|   |1|   |5*3 + 8*4 + 1*7 + 1*1|   |15+32+07+01|   |55|   
    */

    std::shared_ptr<geometry::Point3d> vector = std::make_shared<geometry::Point3d>(3, 4, 7);
    std::shared_ptr<geometry::Point3d> resultVector = std::make_shared<geometry::Point3d>(0, 0, 0);

    float w = 0;
    calculator->multiply(matrix.get(), vector.get(), resultVector.get(), w);

    REQUIRE(std::floor((resultVector->x / w) * 100) == 38);
    REQUIRE(std::floor((resultVector->y / w) * 100) == 74);
    REQUIRE(std::floor((resultVector->z / w)  * 100) == 61);
}

TEST_CASE("multiply matrix by matrix", "[matricies]")
{
    std::shared_ptr<services::IMatrixCalculatorService> calculator = std::make_shared<services::MatrixCalculatorService>();
    std::shared_ptr<geometry::Matrix4x4> matrixLeft = std::make_shared<geometry::Matrix4x4>(geometry::Matrix4x4::EntriesArray {
        1, 2, 1, 3,
        6, 2, 2, 1,
        2, 5, 1, 1,
        5, 8, 1, 1
    });
    std::shared_ptr<geometry::Matrix4x4> matrixRight = std::make_shared<geometry::Matrix4x4>(geometry::Matrix4x4::EntriesArray {
        1, 6, 2, 5,
        2, 2, 5, 8,
        1, 2, 1, 1,
        3, 1, 1, 1
    });
    std::shared_ptr<geometry::Matrix4x4> resultMatrix = std::make_shared<geometry::Matrix4x4>();

    calculator->multiply(matrixLeft.get(), matrixRight.get(), resultMatrix.get());

    /*
    15 15 16 25
    15 45 25 49
    16 25 31 52
    25 49 52 91
    */

   std::shared_ptr<geometry::Matrix4x4> matrixRequire = std::make_shared<geometry::Matrix4x4>(geometry::Matrix4x4::EntriesArray {
        15, 15, 16, 25,
        15, 45, 25, 49,
        16, 25, 31, 52,
        25, 49, 52, 91
    });

   REQUIRE(calculator->isEqual(matrixRequire.get(), resultMatrix.get()));
}

TEST_CASE("inverse matrix", "[matricies]")
{
    std::shared_ptr<services::IMatrixCalculatorService> calculator = std::make_shared<services::MatrixCalculatorService>();
    std::shared_ptr<geometry::Matrix4x4> matrix = std::make_shared<geometry::Matrix4x4>(geometry::Matrix4x4::EntriesArray {
        1, 2, 1, 3,
        6, 2, 2, 1,
        2, 5, 1, 1,
        5, 8, 1, 1
    });
    std::shared_ptr<geometry::Matrix4x4> resultMatrix = std::make_shared<geometry::Matrix4x4>();
    
    calculator->invert(matrix.get(), resultMatrix.get());

    std::shared_ptr<geometry::Matrix4x4> matrixRequire = std::make_shared<geometry::Matrix4x4>(geometry::Matrix4x4::EntriesArray {
        45, 90, -516, 287,
        -46, -91, 181, 45,
        -319, 363, 1606, -1016,
        454, -91, -485, 212
    });

    for(int i = 0; i < geometry::Matrix4x4::size; ++i)
    {
        REQUIRE(std::floor(resultMatrix->entries[i] * 1000) == matrixRequire->entries[i]);
    }
}

TEST_CASE("build translate matrix", "[matricies]")
{
    std::shared_ptr<services::IMatrixCalculatorService> calculator = std::make_shared<services::MatrixCalculatorService>();
    std::shared_ptr<geometry::Matrix4x4> matrix = std::make_shared<geometry::Matrix4x4>(geometry::Matrix4x4::EntriesArray {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    });

    calculator->translate(matrix.get(), 1, 1, 1);

    std::shared_ptr<geometry::Point3d> resultPoint = std::make_shared<geometry::Point3d>(0, 0, 0);
    std::shared_ptr<geometry::Point3d> point = std::make_shared<geometry::Point3d>(1, 1, 1);
    calculator->multiply(matrix.get(), point.get(), resultPoint.get());

    REQUIRE(resultPoint->x == 2);
    REQUIRE(resultPoint->y == 2);
    REQUIRE(resultPoint->z == 2);
}

TEST_CASE("build scale matrix", "[matricies]")
{
    std::shared_ptr<services::IMatrixCalculatorService> calculator = std::make_shared<services::MatrixCalculatorService>();
    std::shared_ptr<geometry::Matrix4x4> matrix = std::make_shared<geometry::Matrix4x4>(geometry::Matrix4x4::EntriesArray {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    });

    calculator->scale(matrix.get(), 2, 2, 2);

    std::shared_ptr<geometry::Point3d> resultPoint = std::make_shared<geometry::Point3d>(0, 0, 0);
    std::shared_ptr<geometry::Point3d> point = std::make_shared<geometry::Point3d>(1, 1, 1);
    calculator->multiply(matrix.get(), point.get(), resultPoint.get());

    REQUIRE(resultPoint->x == 2);
    REQUIRE(resultPoint->y == 2);
    REQUIRE(resultPoint->z == 2);
}

TEST_CASE("build rotate X matrix", "[matricies]")
{
    std::shared_ptr<services::IMatrixCalculatorService> calculator = std::make_shared<services::MatrixCalculatorService>();
    std::shared_ptr<geometry::Matrix4x4> matrix = std::make_shared<geometry::Matrix4x4>(geometry::Matrix4x4::EntriesArray {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    });

    calculator->rotateX(matrix.get(), geometry::MathFunctions::getRadians(90.f));

    std::shared_ptr<geometry::Point3d> resultPoint = std::make_shared<geometry::Point3d>(0, 0, 0);
    std::shared_ptr<geometry::Point3d> point = std::make_shared<geometry::Point3d>(0, 1, 0);
    calculator->multiply(matrix.get(), point.get(), resultPoint.get());

    REQUIRE((int)resultPoint->x == 0);
    REQUIRE((int)resultPoint->y == 0);
    REQUIRE((int)resultPoint->z == 1);
}

TEST_CASE("build rotate Y matrix", "[matricies]")
{
    std::shared_ptr<services::IMatrixCalculatorService> calculator = std::make_shared<services::MatrixCalculatorService>();
    std::shared_ptr<geometry::Matrix4x4> matrix = std::make_shared<geometry::Matrix4x4>(geometry::Matrix4x4::EntriesArray {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    });

    calculator->rotateY(matrix.get(), geometry::MathFunctions::getRadians(90.f));

    std::shared_ptr<geometry::Point3d> resultPoint = std::make_shared<geometry::Point3d>(0, 0, 0);
    std::shared_ptr<geometry::Point3d> point = std::make_shared<geometry::Point3d>(1, 0, 0);
    calculator->multiply(matrix.get(), point.get(), resultPoint.get());

    REQUIRE((int)resultPoint->x == 0);
    REQUIRE((int)resultPoint->y == 0);
    REQUIRE((int)resultPoint->z == -1);
}

TEST_CASE("build rotate Z matrix", "[matricies]")
{
    std::shared_ptr<services::IMatrixCalculatorService> calculator = std::make_shared<services::MatrixCalculatorService>();
    std::shared_ptr<geometry::Matrix4x4> matrix = std::make_shared<geometry::Matrix4x4>(geometry::Matrix4x4::EntriesArray {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    });

    calculator->rotateZ(matrix.get(), geometry::MathFunctions::getRadians(90.f));

    std::shared_ptr<geometry::Point3d> resultPoint = std::make_shared<geometry::Point3d>(0, 0, 0);
    std::shared_ptr<geometry::Point3d> point = std::make_shared<geometry::Point3d>(1, 0, 0);
    calculator->multiply(matrix.get(), point.get(), resultPoint.get());

    REQUIRE((int)resultPoint->x == 0);
    REQUIRE((int)resultPoint->y == 1);
    REQUIRE((int)resultPoint->z == 0);
}