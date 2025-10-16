#pragma once

#include "pageAllocator.h"

namespace dory::memory
{
    template<typename TPageAllocator, typename THugePageAllocator, typename TSlotAllocator>
    class SegregationAllocator
    {
    private:
        TPageAllocator& _blockAllocator;
        THugePageAllocator& _hugeBlockAllocator;
        std::vector<TSlotAllocator>& _slotAllocatorClasses;

    public:
        explicit SegregationAllocator(TPageAllocator& blockAllocator,
            THugePageAllocator& hugeBlockAllocator,
            std::vector<TSlotAllocator>& slotAllocatorClasses) noexcept:
        _blockAllocator(blockAllocator),
        _hugeBlockAllocator(hugeBlockAllocator),
        _slotAllocatorClasses(slotAllocatorClasses)
        {}

        void* allocate(std::size_t size) noexcept
        {

        }

        void deallocate(void* ptr) noexcept
        {

        }
    };
}
