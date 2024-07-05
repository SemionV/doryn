#include "dependencies.h"

using namespace dory::math;

TEST_CASE("get entry position 4x4", "[matricies]")
{
    Matrix4x4f matrix;

    auto position = matrix.getPositionByIndex(0);
    REQUIRE(position.column == 0);
    REQUIRE(position.row    == 0);
    REQUIRE(matrix.isDiagonalEntry(position) == true);

    position = matrix.getPositionByIndex(14);
    REQUIRE(position.column == 2);
    REQUIRE(position.row    == 3);
    REQUIRE(matrix.isDiagonalEntry(position) == false);

    position = matrix.getPositionByIndex(15);
    REQUIRE(position.column == 3);
    REQUIRE(position.row    == 3);
    REQUIRE(matrix.isDiagonalEntry(position) == true);
}

TEST_CASE( "setToIdentity 4x4", "[matricies]" ) 
{
    auto matrix = Matrix4x4f{};

    const int matrixSize = 16;

    for(int i = 0; i < matrixSize; ++i)
    {
        matrix.entries[i] = 0;
    }

    matrix.toIdentity();

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
        auto entryPosition = matrix.getPositionByIndex(i);
        auto entryValue = matrix.entries[i];

        if(matrix.isDiagonalEntry(entryPosition))
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
    auto matrix = Matrix4x4f{};

    const int matrixSize = 16;

    for(int i = 0; i < matrixSize; i++)
    {
        matrix.entries[i] = (float)i;
    }

    auto matrix2 = matrix;

    for(int i = 0; i < matrixSize; ++i)
    {
        REQUIRE(matrix.entries[i] == matrix2.entries[i]);
    }
}

TEST_CASE("multiply vector by matrix", "[matricies]")
{
    auto matrix = Matrix4x4f(Matrix4x4f::EntriesArray {
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

    auto vector = Vector4f {3.0f, 4.0f, 7.0f, 1.0f};

    auto resultVector = matrix.multiply(vector);

    float w = resultVector.w;

    REQUIRE(std::floor((resultVector.x / w) * 100) == 38);
    REQUIRE(std::floor((resultVector.y / w) * 100) == 74);
    REQUIRE(std::floor((resultVector.z / w)  * 100) == 61);
}

TEST_CASE("multiply matrix by matrix", "[matricies]")
{
    auto matrixLeft = Matrix4x4f (Matrix4x4f::EntriesArray {
        1, 2, 1, 3,
        6, 2, 2, 1,
        2, 5, 1, 1,
        5, 8, 1, 1
    });
    auto matrixRight = Matrix4x4f (Matrix4x4f::EntriesArray {
        1, 6, 2, 5,
        2, 2, 5, 8,
        1, 2, 1, 1,
        3, 1, 1, 1
    });
    auto resultMatrix = matrixLeft.multiply(matrixRight);

    /*
    15 15 16 25
    15 45 25 49
    16 25 31 52
    25 49 52 91
    */

    auto matrixRequire = Matrix4x4f(Matrix4x4f::EntriesArray {
        15, 15, 16, 25,
        15, 45, 25, 49,
        16, 25, 31, 52,
        25, 49, 52, 91
    });

   REQUIRE(matrixRequire.isEqual(resultMatrix));
}

TEST_CASE("inverse matrix", "[matricies]")
{
    auto matrix = Matrix4x4f(Matrix4x4f::EntriesArray {
        1, 2, 1, 3,
        6, 2, 2, 1,
        2, 5, 1, 1,
        5, 8, 1, 1
    });
    auto resultMatrix = matrix.getInverted();

    auto matrixRequire = Matrix4x4f(Matrix4x4f::EntriesArray {
        45, 90, -516, 287,
        -46, -91, 181, 45,
        -319, 363, 1606, -1016,
        454, -91, -485, 212
    });

    for(int i = 0; i < Matrix4x4f::EntriesCount; ++i)
    {
        REQUIRE(std::floor(resultMatrix.entries[i] * 1000) == matrixRequire.entries[i]);
    }
}

TEST_CASE("build translate matrix", "[matricies]")
{
    auto matrix = Matrix4x4f(Matrix4x4f::EntriesArray {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    });

    matrix.translate(Vector3f{1, 1, 1});

    auto resultPoint = matrix.multiply(Vector3f{1, 1, 1});

    REQUIRE(resultPoint.x == 2);
    REQUIRE(resultPoint.y == 2);
    REQUIRE(resultPoint.z == 2);
}

TEST_CASE("build scale matrix", "[matricies]")
{
    auto matrix = Matrix4x4f(Matrix4x4f::EntriesArray {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    });

    matrix.scale(Vector3f{2, 2, 2});

    auto resultPoint = matrix.multiply(Vector3f{1, 1, 1});

    REQUIRE(resultPoint.x == 2);
    REQUIRE(resultPoint.y == 2);
    REQUIRE(resultPoint.z == 2);
}

TEST_CASE("build rotate X matrix", "[matricies]")
{
    auto matrix = Matrix4x4f(Matrix4x4f::EntriesArray {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    });

    matrix.rotateX(getRadians(90.f));

    auto resultPoint = matrix.multiply(Vector3f{0, 1, 0});

    REQUIRE((int)resultPoint.x == 0);
    REQUIRE((int)resultPoint.y == 0);
    REQUIRE((int)resultPoint.z == 1);
}

TEST_CASE("build rotate Y matrix", "[matricies]")
{
    auto matrix = Matrix4x4f (Matrix4x4f::EntriesArray {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    });

    matrix.rotateY(getRadians(90.f));

    auto resultPoint = matrix.multiply(Vector3f{1, 0, 0});

    REQUIRE((int)resultPoint.x == 0);
    REQUIRE((int)resultPoint.y == 0);
    REQUIRE((int)resultPoint.z == -1);
}

TEST_CASE("build rotate Z matrix", "[matricies]")
{
    auto matrix = Matrix4x4f(Matrix4x4f::EntriesArray {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    });

    matrix.rotateZ(getRadians(90.f));

    auto resultPoint = matrix.multiply(Vector3f{1, 0, 0});

    REQUIRE((int)resultPoint.x == 0);
    REQUIRE((int)resultPoint.y == 1);
    REQUIRE((int)resultPoint.z == 0);
}