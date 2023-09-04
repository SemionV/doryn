#pragma once

#include "point.h"

namespace dory
{
    struct Matrix4x4
    {
        float entries[16];

        Matrix4x4():
            entries{1.f, 0.f, 0.f, 0.f,
                    0.f, 1.f, 0.f, 0.f,
                    0.f, 0.f, 1.f, 0.f,
                    0.f, 0.f, 0.f, 1.f}
        {            
        }

        enum
        {
            size = 16
        };

        static const dory::Point2d getEntryPositionByIndex(const int index)
        {
            const int row = index / 4;
            return Point2d(index - row * 4, row);
        }

        static const bool isDiagonalEntry(const Point2d& position)
        {
            return position.x == position.y;
        }

        static void setEntries(Matrix4x4* matrix, float entries[size])
        {
            for(int i = 0; i < size; ++i)
            {
                matrix->entries[i] = entries[i];
            }
        }

        static const bool isEqual(Matrix4x4* matrixA, Matrix4x4* matrixB)
        {
            for(int i = 0; i < size; ++i)
            {
                if(matrixA->entries[i] != matrixB->entries[i])
                {
                    return false;
                }
            }

            return true;
        }
    };
}