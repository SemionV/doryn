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

        float m00 = lhs[0] * rhs[0] + lhs[1] * rhs[4] + lhs[2] * rhs[8] + lhs[3] * rhs[12];
        float m01 = lhs[4] * rhs[0] + lhs[5] * rhs[4] + lhs[6] * rhs[8] + lhs[7] * rhs[12];
        float m02 = lhs[8] * rhs[0] + lhs[9] * rhs[4] + lhs[10] * rhs[8] + lhs[11] * rhs[12];
        float m03 = lhs[12] * rhs[0] + lhs[13] * rhs[4] + lhs[14] * rhs[8] + lhs[15] * rhs[12];

        float m10 = lhs[0] * rhs[1] + lhs[1] * rhs[5] + lhs[2] * rhs[9] + lhs[3] * rhs[13];
        float m11 = lhs[4] * rhs[1] + lhs[5] * rhs[5] + lhs[6] * rhs[9] + lhs[7] * rhs[13];
        float m12 = lhs[8] * rhs[1] + lhs[9] * rhs[5] + lhs[10] * rhs[9] + lhs[11] * rhs[13];
        float m13 = lhs[12] * rhs[1] + lhs[13] * rhs[5] + lhs[14] * rhs[9] + lhs[15] * rhs[13];

        float m20 = lhs[0] * rhs[2] + lhs[1] * rhs[6] + lhs[2] * rhs[10] + lhs[3] * rhs[14];
        float m21 = lhs[4] * rhs[2] + lhs[5] * rhs[6] + lhs[6] * rhs[10] + lhs[7] * rhs[14];
        float m22 = lhs[8] * rhs[2] + lhs[9] * rhs[6] + lhs[10] * rhs[10] + lhs[11] * rhs[14];
        float m23 = lhs[12] * rhs[2] + lhs[13] * rhs[6] + lhs[14] * rhs[10] + lhs[15] * rhs[14];

        float m30 = lhs[0] * rhs[3] + lhs[1] * rhs[7] + lhs[2] * rhs[11] + lhs[3] * rhs[15];
        float m31 = lhs[4] * rhs[3] + lhs[5] * rhs[7] + lhs[6] * rhs[11] + lhs[7] * rhs[15];
        float m32 = lhs[8] * rhs[3] + lhs[9] * rhs[7] + lhs[10] * rhs[11] + lhs[11] * rhs[15];
        float m33 = lhs[12] * rhs[3] + lhs[13] * rhs[7] + lhs[14] * rhs[11] + lhs[15] * rhs[15];

        float* r = resultMatrix->entries;
        r[0] = m00; r[1] = m10; r[2] = m20; r[3] = m30;
        r[4] = m01; r[5] = m11; r[6] = m21; r[7] = m31;
        r[8] = m02; r[9] = m12; r[10] = m22; r[11] = m32;
        r[12] = m03; r[13] = m13; r[14] = m23; r[15] = m33;
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