#include "matrixCalculatorService.h"

namespace dory::domain::services
{
    void MatrixCalculatorService::setToIdentity(geometry::Matrix4x4* matrix)
    {
        auto entries = matrix->entries;

        const float f1 = 1.f;
        const float f0 = 0.f;

        entries[0] = f1; entries[1] = f0; entries[2] = f0; entries[3] = f0;
        entries[4] = f0; entries[5] = f1; entries[6] = f0; entries[7] = f0;
        entries[8] = f0; entries[9] = f0; entries[10] = f1; entries[11] = f0;
        entries[12] = f0; entries[13] = f0; entries[14] = f0; entries[15] = f1;
    }

    void MatrixCalculatorService::copyEntriesTo(const geometry::Matrix4x4* sourceMatrix, geometry::Matrix4x4* destinationMatrix)
    {
        for(int i = 0; i < geometry::Matrix4x4::size; ++i)
        {
            destinationMatrix->entries[i] = sourceMatrix->entries[i];
        }
    }

    void MatrixCalculatorService::multiply(const geometry::Matrix4x4* matrix, const geometry::Point3d* vector, geometry::Point3d* resultVector)
    {
        const float* entries = matrix->entries;

        float x = (vector->x * entries[0]) + (vector->y * entries[1]) + (vector->z * entries[2]) + entries[3];
        float y = (vector->x * entries[4]) + (vector->y * entries[5]) + (vector->z * entries[6]) + entries[7];
        float z = (vector->x * entries[8]) + (vector->y * entries[9]) + (vector->z * entries[10]) + entries[11];

        resultVector->x = x;
        resultVector->y = y;
        resultVector->z = z;
    }

    void MatrixCalculatorService::multiply(const geometry::Matrix4x4* matrix, const geometry::Point3d* vector, geometry::Point3d* resultVector, float& wResult)
    {
        multiply(matrix, vector, resultVector);

        const float* entries = matrix->entries;
        wResult = (vector->x * entries[12]) + (vector->y * entries[13]) + (vector->z * entries[14]) + entries[15];
    }

    void MatrixCalculatorService::multiply(const geometry::Matrix4x4* matrixLeftSide, const geometry::Matrix4x4* matrixRightSide, geometry::Matrix4x4* resultMatrix)
    {
        const float* lhs = matrixLeftSide->entries;
        const float* rhs = matrixRightSide->entries;

        float a00 = lhs[0], a01 = lhs[1], a02 = lhs[2], a03 = lhs[3],
              a10 = lhs[4], a11 = lhs[5], a12 = lhs[6], a13 = lhs[7],
              a20 = lhs[8], a21 = lhs[9], a22 = lhs[10], a23 = lhs[11],
              a30 = lhs[12], a31 = lhs[13], a32 = lhs[14], a33 = lhs[15];

        float b00 = rhs[0], b01 = rhs[1], b02 = rhs[2], b03 = rhs[3],
              b10 = rhs[4], b11 = rhs[5], b12 = rhs[6], b13 = rhs[7],
              b20 = rhs[8], b21 = rhs[9], b22 = rhs[10], b23 = rhs[11],
              b30 = rhs[12], b31 = rhs[13], b32 = rhs[14], b33 = rhs[15];

        float m00 = a00 * b00 + a01 * b10 + a02 * b20 + a03 * b30;
        float m10 = a10 * b00 + a11 * b10 + a12 * b20 + a13 * b30;
        float m20 = a20 * b00 + a21 * b10 + a22 * b20 + a23 * b30;
        float m30 = a30 * b00 + a31 * b10 + a32 * b20 + a33 * b30;

        float m01 = a00 * b01 + a01 * b11 + a02 * b21 + a03 * b31;
        float m11 = a10 * b01 + a11 * b11 + a12 * b21 + a13 * b31;
        float m21 = a20 * b01 + a21 * b11 + a22 * b21 + a23 * b31;
        float m31 = a30 * b01 + a31 * b11 + a32 * b21 + a33 * b31;

        float m02 = a00 * b02 + a01 * b12 + a02 * b22 + a03 * b32;
        float m12 = a10 * b02 + a11 * b12 + a12 * b22 + a13 * b32;
        float m22 = a20 * b02 + a21 * b12 + a22 * b22 + a23 * b32;
        float m32 = a30 * b02 + a31 * b12 + a32 * b22 + a33 * b32;

        float m03 = a00 * b03 + a01 * b13 + a02 * b23 + a03 * b33;
        float m13 = a10 * b03 + a11 * b13 + a12 * b23 + a13 * b33;
        float m23 = a20 * b03 + a21 * b13 + a22 * b23 + a23 * b33;
        float m33 = a30 * b03 + a31 * b13 + a32 * b23 + a33 * b33;

        float* r = resultMatrix->entries;
        r[0] = m00; r[1] = m01; r[2] = m02; r[3] = m03;
        r[4] = m10; r[5] = m11; r[6] = m12; r[7] = m13;
        r[8] = m20; r[9] = m21; r[10] = m22; r[11] = m23;
        r[12] = m30; r[13] = m31; r[14] = m32; r[15] = m33;
    }

    void MatrixCalculatorService::invert(const geometry::Matrix4x4* matrix, geometry::Matrix4x4* resultMatrix)
    {
        const float* m = matrix->entries;

        float a00 = m[0], a01 = m[1], a02 = m[2], a03 = m[3],
            a10 = m[4], a11 = m[5], a12 = m[6], a13 = m[7],
            a20 = m[8], a21 = m[9], a22 = m[10], a23 = m[11],
            a30 = m[12], a31 = m[13], a32 = m[14], a33 = m[15],
            b00 = a00 * a11 - a01 * a10,
            b01 = a00 * a12 - a02 * a10,
            b02 = a00 * a13 - a03 * a10,
            b03 = a01 * a12 - a02 * a11,
            b04 = a01 * a13 - a03 * a11,
            b05 = a02 * a13 - a03 * a12,
            b06 = a20 * a31 - a21 * a30,
            b07 = a20 * a32 - a22 * a30,
            b08 = a20 * a33 - a23 * a30,
            b09 = a21 * a32 - a22 * a31,
            b10 = a21 * a33 - a23 * a31,
            b11 = a22 * a33 - a23 * a32,

            det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

        if (det) {
            float* result = resultMatrix->entries;

            det = 1.0 / det;
            result[0] = (a11 * b11 - a12 * b10 + a13 * b09) * det;
            result[1] = (a02 * b10 - a01 * b11 - a03 * b09) * det;
            result[2] = (a31 * b05 - a32 * b04 + a33 * b03) * det;
            result[3] = (a22 * b04 - a21 * b05 - a23 * b03) * det;
            result[4] = (a12 * b08 - a10 * b11 - a13 * b07) * det;
            result[5] = (a00 * b11 - a02 * b08 + a03 * b07) * det;
            result[6] = (a32 * b02 - a30 * b05 - a33 * b01) * det;
            result[7] = (a20 * b05 - a22 * b02 + a23 * b01) * det;
            result[8] = (a10 * b10 - a11 * b08 + a13 * b06) * det;
            result[9] = (a01 * b08 - a00 * b10 - a03 * b06) * det;
            result[10] = (a30 * b04 - a31 * b02 + a33 * b00) * det;
            result[11] = (a21 * b02 - a20 * b04 - a23 * b00) * det;
            result[12] = (a11 * b07 - a10 * b09 - a12 * b06) * det;
            result[13] = (a00 * b09 - a01 * b07 + a02 * b06) * det;
            result[14] = (a31 * b01 - a30 * b03 - a32 * b00) * det;
            result[15] = (a20 * b03 - a21 * b01 + a22 * b00) * det;
        }
    }

    void MatrixCalculatorService::toIdentity(geometry::Matrix4x4* matrix)
    {
        geometry::Matrix4x4::setEntries(matrix, geometry::Matrix4x4::EntriesArray {
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f
        });
    }

    void MatrixCalculatorService::translate(geometry::Matrix4x4* matrix, float dx, float dy, float dz)
    {
        matrix->entries[3] = dx;
        matrix->entries[7] = dy;
        matrix->entries[11] = dz;
    }

    void MatrixCalculatorService::scale(geometry::Matrix4x4* matrix, float x, float y, float z)
    {
        matrix->entries[0] = x;
        matrix->entries[5] = y;
        matrix->entries[10] = z;
    }

    void MatrixCalculatorService::rotateX(geometry::Matrix4x4* matrix, float radians)
    {
        matrix->entries[5] = std::cos(radians);
        matrix->entries[6] = -std::sin(radians);
        matrix->entries[9] = std::sin(radians);
        matrix->entries[10] = std::cos(radians);
    }

    void MatrixCalculatorService::rotateY(geometry::Matrix4x4* matrix, float radians)
    {
        matrix->entries[0] = std::cos(radians);
        matrix->entries[2] = std::sin(radians);
        matrix->entries[8] = -std::sin(radians);
        matrix->entries[10] = std::cos(radians);
    }

    void MatrixCalculatorService::rotateZ(geometry::Matrix4x4* matrix, float radians)
    {
        matrix->entries[0] = std::cos(radians);
        matrix->entries[1] = -std::sin(radians);
        matrix->entries[4] = std::sin(radians);
        matrix->entries[5] = std::cos(radians);
    }

    bool MatrixCalculatorService::isEqual(geometry::Matrix4x4* matrixA, geometry::Matrix4x4* matrixB)
    {
        for(int i = 0; i < geometry::Matrix4x4::size; ++i)
        {
            if(matrixA->entries[i] != matrixB->entries[i])
            {
                return false;
            }
        }

        return true;
    }
}