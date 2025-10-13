#pragma once
#include <atomic>
#include <cstddef>
#include "blockAllocator.h"

namespace dory::memory
{
    template<typename TBlockAllocator>
    class FreeListAllocator
    {
    private:
        const std::size_t _cellSize;
        const std::size_t _cellsPerChunkCount;
        TBlockAllocator& _blockAllocator;
        std::atomic<void*> _freeListHead = nullptr;
        MemoryBlock _memoryBlock;

    public:
        FreeListAllocator(const std::size_t cellSize, const std::size_t cellsPerChunkCount, TBlockAllocator& blockAllocator) noexcept:
        _cellSize(cellSize),
        _cellsPerChunkCount(cellsPerChunkCount),
        _blockAllocator(blockAllocator)
        {
            assert::debug(cellSize > 0, "CellSize must be greater than zero");
            assert::debug(cellSize > sizeof(void*), "CellSize must be at least as big as size of a pointer");
            assert::debug(cellsPerChunkCount > 0, "CellsPerChunkCount must be greater than zero");
            const std::size_t cellsPerPageCount = _blockAllocator.getPageSize() / _cellSize;
            assert::debug(cellsPerChunkCount % cellsPerPageCount == 0, "Cell per chunk must be a multiple of cells per page");
            assert::debug(_blockAllocator.getPageSize() >= cellSize, "CellSize must fit into a memory page");
            const std::size_t mask = cellSize - 1;
            assert::debug((cellSize & mask) == 0, "CellSize must be a power of 2");

            allocateChunk();
        }

        ~FreeListAllocator()
        {
            if(_memoryBlock.ptr != nullptr)
            {
                _blockAllocator.deallocate(_memoryBlock);
            }
        }

        void* allocate(const std::size_t size) noexcept
        {
            assert::debug(size <= _cellSize, "Requested memory allocation is larger than a memory cell");

            while(true)
            {
                void* headPointer = _freeListHead.load(std::memory_order::relaxed);

                if(headPointer != nullptr)
                {
                    void* nextCellPointer = *static_cast<void* const*>(headPointer);
                    if(_freeListHead.compare_exchange_weak(headPointer, nextCellPointer, std::memory_order::acquire, std::memory_order::relaxed))
                    {
                        return headPointer;
                    }

                    continue;
                }

                assert::release(false, "Out of memory");
                return nullptr;
            }
        }

        void deallocate(void* ptr) noexcept
        {
            const auto chunkStartAddress = reinterpret_cast<uintptr_t>(_memoryBlock.ptr);
            const std::uintptr_t chunkEndAddress = chunkStartAddress + _memoryBlock.pageSize * _memoryBlock.pagesCount;
            const auto address = reinterpret_cast<uintptr_t>(ptr);
            assert::inhouse(address >= chunkStartAddress && address <= chunkEndAddress - sizeof(void*), "Pointer does not belong to the managed memory block");

            while(true)
            {
                void* headPointer = _freeListHead.load(std::memory_order::relaxed);

                *static_cast<void**>(ptr) = headPointer;

                if(_freeListHead.compare_exchange_weak(headPointer, ptr, std::memory_order::release, std::memory_order::relaxed)) [[likely]]
                {
                    break;
                }
            }
        }

    private:
        void allocateChunk()
        {
            const std::size_t cellsPerPageCount = _blockAllocator.getPageSize() / _cellSize;
            const std::size_t pagesCount = _cellsPerChunkCount / cellsPerPageCount;

            const ErrorCode errorCode = _blockAllocator.allocate(pagesCount, _memoryBlock);
            if(errorCode != ErrorCode::Success)
            {
                assert::release(false, "Cannot allocate memory block");
            }

            for(std::size_t i = 0; i < _cellsPerChunkCount; ++i)
            {
                const std::uintptr_t cellAddress = reinterpret_cast<std::uintptr_t>(_memoryBlock.ptr) + _cellSize * i;

                if(i != _cellsPerChunkCount - 1)
                {
                    const std::uintptr_t nextCellAddress = reinterpret_cast<std::uintptr_t>(_memoryBlock.ptr) + _cellSize * (i + 1);
                    *reinterpret_cast<std::uintptr_t*>(cellAddress) = nextCellAddress;
                }
                else
                {
                    *reinterpret_cast<void**>(cellAddress) = nullptr;
                }
            }

            _freeListHead = _memoryBlock.ptr;
        }
    };
}
