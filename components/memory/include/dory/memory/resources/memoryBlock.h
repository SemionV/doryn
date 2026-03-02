#pragma once

#include <atomic>

namespace dory::memory
{
    struct MemoryBlock
    {
        void* ptr {};
        std::size_t pagesCount {};
        std::size_t pageSize {};
        std::size_t commitedPagesCount {};
        std::atomic<std::size_t> index = 0;
    };

    struct MemoryBlockNode
    {
        void* data {};
        std::atomic<std::size_t> index = 0;
        MemoryBlockNode* previousNode = nullptr;
    };
}