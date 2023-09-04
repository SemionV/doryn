#pragma once

#include "matrixCalculatorInterface.h"

namespace dory
{
    class MatrixCalculator: public IMatrixCalculator
    {
        public:
            void setToIdentity(Matrix4x4* matrix) override;
            void copyEntriesTo(const Matrix4x4* sourceMatrix, Matrix4x4* destinationMatrix) override;
            void multiply(const Matrix4x4* matrix, const Point3d* vector, Point3d* resultVector) override;
            void multiply(const Matrix4x4* matrixLeftSide, const Matrix4x4* matrixRightSide, Matrix4x4* resultMatrix) override;
            void invert(const Matrix4x4* matrix, Matrix4x4* resultMatrix) override;
            void translate(Matrix4x4* matrix, float x, float y, float z) override;
            void scale(Matrix4x4* matrix, float x, float y, float z) override;
            void rotateX(Matrix4x4* matrix, float radians) override;
            void rotateY(Matrix4x4* matrix, float radians) override;
            void rotateZ(Matrix4x4* matrix, float radians) override;
    };
}