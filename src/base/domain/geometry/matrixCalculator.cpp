#include "matrixCalculator.h"

namespace dory
{
    void MatrixCalculator::setToIdentity(Matrix4x4* matrix)
    {
        auto entries = matrix->entries;

        const float f1 = 1.f;
        const float f0 = 0.f;

        entries[0] = f1; entries[1] = f0; entries[2] = f0; entries[3] = f0;
        entries[4] = f0; entries[5] = f1; entries[6] = f0; entries[7] = f0;
        entries[8] = f0; entries[9] = f0; entries[10] = f1; entries[11] = f0;
        entries[12] = f0; entries[13] = f0; entries[14] = f0; entries[15] = f1;
    }

    void MatrixCalculator::copyEntriesTo(const Matrix4x4* sourceMatrix, Matrix4x4* destinationMatrix)
    {
        for(int i = 0; i < Matrix4x4::size; ++i)
        {
            destinationMatrix->entries[i] = sourceMatrix->entries[i];
        }
    }

    void MatrixCalculator::multiply(const Matrix4x4* matrix, const Point3d* vector, Point3d* resultVector)
    {
        const float* entries = matrix->entries;

        float x = (vector->x * entries[0]) + (vector->y * entries[1]) + (vector->z * entries[2]) + entries[3];
        float y = (vector->x * entries[4]) + (vector->y * entries[5]) + (vector->z * entries[6]) + entries[7];
        float z = (vector->x * entries[8]) + (vector->y * entries[9]) + (vector->z * entries[10]) + entries[11];
        float w = (vector->x * entries[12]) + (vector->y * entries[13]) + (vector->z * entries[14]) + 1;

        resultVector->x = x / w;
        resultVector->y = y / w;
        resultVector->z = z / w;
    }

    void MatrixCalculator::multiply(const Matrix4x4* matrixLeftSide, const Matrix4x4* matrixRightSide, Matrix4x4* resultMatrix)
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

    void MatrixCalculator::invert(const Matrix4x4* matrix, Matrix4x4* resultMatrix)
    {

    }

    void MatrixCalculator::translate(Matrix4x4* matrix, float x, float y, float z)
    {
        
    }

    void MatrixCalculator::scale(Matrix4x4* matrix, float x, float y, float z)
    {
        
    }

    void MatrixCalculator::rotateX(Matrix4x4* matrix, float radians)
    {
        
    }

    void MatrixCalculator::rotateY(Matrix4x4* matrix, float radians)
    {
        
    }

    void MatrixCalculator::rotateZ(Matrix4x4* matrix, float radians)
    {
        
    }
}