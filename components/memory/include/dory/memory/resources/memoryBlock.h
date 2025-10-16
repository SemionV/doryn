#pragma once

namespace dory::memory
{
    struct MemoryBlock
    {
        void* ptr {};
        std::size_t pagesCount {};
        std::size_t pageSize {};
        std::size_t commitedPagesCount {};
    };

    struct MemoryBlockNode
    {
        MemoryBlock memoryBlock;
        MemoryBlockNode* previousNode = nullptr;
    };
}