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
        
    }

    void MatrixCalculator::multiply(const Matrix4x4* matrix, const Point3d* point, Point3d* resultPoint)
    {
        
    }

    void MatrixCalculator::multiply(const Matrix4x4* matrixLeftSide, const Matrix4x4* matrixRightSide, Matrix4x4* resultMatrix)
    {
        
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