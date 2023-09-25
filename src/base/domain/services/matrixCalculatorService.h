#pragma once

#include "base/domain/geometry/matrix.h"
#include "base/doryExport.h"

namespace dory::domain::services
{
    class IMatrixCalculatorService
    {
        public:
            virtual void setToIdentity(geometry::Matrix4x4* matrix) = 0;
            virtual void copyEntriesTo(const geometry::Matrix4x4* sourceMatrix, geometry::Matrix4x4* destinationMatrix) = 0;
            virtual void multiply(const geometry::Matrix4x4* matrixLeftSide, const geometry::Matrix4x4* matrixRightSide, geometry::Matrix4x4* resultMatrix) = 0;
            virtual void multiply(const geometry::Matrix4x4* matrix, const geometry::Point3d* vector, geometry::Point3d* resultVector) = 0;
            virtual void multiply(const geometry::Matrix4x4* matrix, const geometry::Point3d* vector, geometry::Point3d* resultVector, float& wValueResult) = 0;
            virtual void invert(const geometry::Matrix4x4* matrix, geometry::Matrix4x4* resultMatrix) = 0;
            virtual void toIdentity(geometry::Matrix4x4* matrix) = 0;
            virtual void translate(geometry::Matrix4x4* matrix, float dx, float dy, float dz) = 0;
            virtual void scale(geometry::Matrix4x4* matrix, float x, float y, float z) = 0;
            virtual void rotateX(geometry::Matrix4x4* matrix, float radians) = 0;
            virtual void rotateY(geometry::Matrix4x4* matrix, float radians) = 0;
            virtual void rotateZ(geometry::Matrix4x4* matrix, float radians) = 0;
            virtual bool isEqual(geometry::Matrix4x4* matrixA, geometry::Matrix4x4* matrixB) = 0;
    };

    class DORY_API MatrixCalculatorService: public IMatrixCalculatorService
    {
        public:
            void setToIdentity(geometry::Matrix4x4* matrix) override;
            void copyEntriesTo(const geometry::Matrix4x4* sourceMatrix, geometry::Matrix4x4* destinationMatrix) override;
            void multiply(const geometry::Matrix4x4* matrixLeftSide, const geometry::Matrix4x4* matrixRightSide, geometry::Matrix4x4* resultMatrix) override;
            void multiply(const geometry::Matrix4x4* matrix, const geometry::Point3d* vector, geometry::Point3d* resultVector) override;
            void multiply(const geometry::Matrix4x4* matrix, const geometry::Point3d* vector, geometry::Point3d* resultVector, float& wValueResult) override;
            void invert(const geometry::Matrix4x4* matrix, geometry::Matrix4x4* resultMatrix) override;
            void toIdentity(geometry::Matrix4x4* matrix) override;
            void translate(geometry::Matrix4x4* matrix, float dx, float dy, float dz) override;
            void scale(geometry::Matrix4x4* matrix, float x, float y, float z) override;
            void rotateX(geometry::Matrix4x4* matrix, float radians) override;
            void rotateY(geometry::Matrix4x4* matrix, float radians) override;
            void rotateZ(geometry::Matrix4x4* matrix, float radians) override;
            bool isEqual(geometry::Matrix4x4* matrixA, geometry::Matrix4x4* matrixB) override;
    };
}