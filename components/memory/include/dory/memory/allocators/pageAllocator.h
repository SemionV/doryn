#pragma once

#include <dory/macros/assert.h>
#include <dory/memory/resources/memoryBlock.h>
#include "../allocation.h"
#include "../profilers/iBlockAllocProfiler.h"

namespace dory::memory
{
    class PageAllocator
    {
    private:
        const std::size_t _pageSize {};
        profilers::IBlockAllocProfiler* _profiler;

    public:
        explicit PageAllocator(std::size_t pageSize, profilers::IBlockAllocProfiler* profiler) noexcept;

        ErrorCode allocate(std::size_t pagesCount, MemoryBlock& memoryBlock) const noexcept;
        void deallocate(const MemoryBlock& memoryBlock) const noexcept;
        std::size_t getPageSize() const noexcept;
        static void commitPages(const MemoryBlock& memoryBlock, std::size_t pagesCount);
    };
}
