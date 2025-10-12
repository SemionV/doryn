#pragma once

#include <dory/macros/assert.h>
#include <dory/memory/resources/memoryBlock.h>
#include "../allocation.h"

namespace dory::memory
{
    class BlockAllocator
    {
    private:
        const std::size_t _pageSize {};

    public:
        explicit BlockAllocator(std::size_t pageSize) noexcept;

        ErrorCode allocate(std::size_t pagesCount, MemoryBlock& memoryBlock) const noexcept;
        void deallocate(const MemoryBlock& memoryBlock) const noexcept;
        std::size_t getPageSize() const noexcept;
        static void commitPages(const MemoryBlock& memoryBlock, std::size_t pagesCount);
    };
}
