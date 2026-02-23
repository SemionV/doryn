#pragma once

#include "blockAllocator.h"

namespace dory::memory::allocators::specific
{
    class PageBlockAllocator: public BlockAllocator<PageBlockAllocator>
    {
        friend class BlockAllocator;

    private:
        const std::size_t _pageSize;

    public:
        explicit PageBlockAllocator(std::size_t pageSize, profilers::IBlockAllocationProfiler* profiler);

    private:
        [[nodiscard]] void* allocateBlockImpl(LabelType label, std::size_t multiple) const;
        void deallocateBlockImpl(void* ptr, std::size_t multiple) const;
        [[nodiscard]] std::size_t getBlockSizeImpl() const;
    };
}