#pragma once

#include "pageAllocator.h"

namespace dory::memory
{
    class SegregationAllocator
    {
    private:
        PageAllocator& _blockAllocator;
        PageAllocator& _hugeBlockAllocator;

    public:
        explicit SegregationAllocator(PageAllocator& blockAllocator, PageAllocator& hugeBlockAllocator) noexcept;

        void* allocate(std::size_t size) noexcept;
        void deallocate(void* ptr) noexcept;
    };
}
