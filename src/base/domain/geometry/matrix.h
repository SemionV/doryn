#pragma once

#include "base/dependencies.h"
#include "point.h"

namespace dory::domain::geometry
{
    template<typename T, typename U>
    struct Matrix;

    template<typename T, std::size_t Size>
    struct Matrix<T, std::integral_constant<std::size_t, Size>>
    {};

    template<typename T>
    struct Matrix<T, std::integral_constant<std::size_t, 16>>
    {
        enum{ size = 16};

        using EntriesArray = std::array<T, size>;

        struct EntryPosition
        {
            unsigned char row;
            unsigned char column;

            EntryPosition(unsigned char row, unsigned char column):
                    row(row),
                    column(column)
            {
            }
        };

        T entries[size];

        explicit Matrix(EntriesArray&& initEntries):
        entries{initEntries[0], initEntries[1], initEntries[2], initEntries[3],
                    initEntries[4], initEntries[5], initEntries[6], initEntries[7],
                    initEntries[8], initEntries[9], initEntries[10], initEntries[11],
                    initEntries[12], initEntries[13], initEntries[14], initEntries[15]}
        {
        }

        Matrix() = default;

        static EntryPosition getEntryPositionByIndex(const std::size_t index)
        {
            const auto row = index / 4;
            return EntryPosition(row, index - row * 4);
        }

        static bool isDiagonalEntry(const EntryPosition& position)
        {
            return position.row == position.column;
        }

        static void setEntries(Matrix& matrix, EntriesArray&& entries)
        {
            for(int i = 0; i < size; ++i)
            {
                matrix.entries[i] = entries[i];
            }
        }
    };

    using Matrix4x4Size = std::integral_constant<std::size_t, 16>;
    using Matrix4x4f = Matrix<float, Matrix4x4Size>;
    using Matrix4x4d = Matrix<double, Matrix4x4Size>;
    using Matrix4x4i = Matrix<int, Matrix4x4Size>;
}