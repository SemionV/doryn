#pragma once
#include <atomic>
#include <cstddef>
#include "blockAllocator.h"

namespace dory::memory
{
    class FreeListAllocator
    {
    private:
        const std::size_t _cellSize;
        const std::size_t _cellsPerChunkCount;
        BlockAllocator& _blockAllocator;
        std::atomic<void*> _freeListHead = nullptr;
        MemoryBlock _memoryBlock;

    public:
        explicit FreeListAllocator(std::size_t cellSize, std::size_t cellsPerChunkCount, BlockAllocator& blockAllocator) noexcept;
        ~FreeListAllocator();

        void* allocate(std::size_t size) noexcept;
        void deallocate(void* ptr) noexcept;

    private:
        void allocateChunk();
    };
}
