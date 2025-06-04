#pragma once

#include "blockAllocator.h"

namespace dory::memory
{
    class SegregationAllocator
    {
    private:
        BlockAllocator& _blockAllocator;
        BlockAllocator& _hugeBlockAllocator;

    public:
        explicit SegregationAllocator(BlockAllocator& blockAllocator, BlockAllocator& hugeBlockAllocator) noexcept;

        void* allocate(std::size_t size) noexcept;
        void deallocate(void* ptr) noexcept;
    };
}
