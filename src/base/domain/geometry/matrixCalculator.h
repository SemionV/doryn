#pragma once

#include "matrixCalculatorInterface.h"

namespace dory
{
    class MatrixCalculator: public IMatrixCalculator
    {
        public:
            void setToIdentity(Matrix4x4* matrix) override;
            void copyEntriesTo(const Matrix4x4* sourceMatrix, Matrix4x4* destinationMatrix) override;
            void multiply(const Matrix4x4* matrix, const Point3d* point, Point3d* resultPoint) override;
            void multiply(const Matrix4x4* matrixLeftSide, const Matrix4x4* matrixRightSide, Matrix4x4* resultMatrix) override;
            void invert(const Matrix4x4* matrix, Matrix4x4* resultMatrix) override;
            void translate(Matrix4x4* matrix, float x, float y, float z) override;
            void scale(Matrix4x4* matrix, float x, float y, float z) override;
            void rotateX(Matrix4x4* matrix, float radians) override;
            void rotateY(Matrix4x4* matrix, float radians) override;
            void rotateZ(Matrix4x4* matrix, float radians) override;

            static const dory::Point2d getMatrix4x4PositionByIndex(const int index)
            {
                const int row = index / 4;
                return Point2d(index - row * 4, row);
            }

            static const bool isMatrix4x4DiagonalEntry(const Point2d& position)
            {
                return position.x == position.y;
            }
    };
}