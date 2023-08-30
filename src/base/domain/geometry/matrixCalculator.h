#pragma once

#include "matrix.h"
#include "point.h"

namespace dory
{
    class IMatrixCalculator
    {
        public:
            virtual void setToIdentity(Matrix4x4* matrix) = 0;
            virtual void copyEntriesTo(const Matrix4x4* sourceMatrix, Matrix4x4* destinationMatrix) = 0;
            virtual void multiply(const Matrix4x4* matrix, const Point3d* point, Point3d* resultPoint) = 0;
            virtual void multiply(const Matrix4x4* matrixA, const Matrix4x4* matrixB, Matrix4x4* resultMatrix) = 0;
            virtual void invert(const Matrix4x4* matrix, Matrix4x4* resultMatrix) = 0;
            virtual void translate(Matrix4x4* matrix, float x, float y, float z) = 0;
            virtual void scale(Matrix4x4* matrix, float x, float y, float z) = 0;
            virtual void rotateX(Matrix4x4* matrix, float radians) = 0;
            virtual void rotateY(Matrix4x4* matrix, float radians) = 0;
            virtual void rotateZ(Matrix4x4* matrix, float radians) = 0;
    };
}