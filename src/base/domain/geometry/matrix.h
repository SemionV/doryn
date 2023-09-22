#pragma once

#include "base/dependencies.h"
#include "point.h"

namespace dory::domain::geometry
{
    struct Matrix4x4
    {        
        enum 
        {
            size = 16
        };

        using EntriesArray = std::array<float, size>;

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

        float entries[size];

        Matrix4x4(EntriesArray&& initEntries):
            entries{initEntries[0], initEntries[1], initEntries[2], initEntries[3],
                    initEntries[4], initEntries[5], initEntries[6], initEntries[7],
                    initEntries[8], initEntries[9], initEntries[10], initEntries[11],
                    initEntries[12], initEntries[13], initEntries[14], initEntries[15]}
        {            
        }

        Matrix4x4()
        {            
        }

        static const EntryPosition getEntryPositionByIndex(const int index)
        {
            const int row = index / 4;
            return EntryPosition(row, index - row * 4);
        }

        static const bool isDiagonalEntry(const EntryPosition& position)
        {
            return position.row == position.column;
        }

        static void setEntries(Matrix4x4* matrix, EntriesArray&& entries)
        {
            for(int i = 0; i < size; ++i)
            {
                matrix->entries[i] = entries[i];
            }
        }
    };
}