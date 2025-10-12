#include <dory/memory/allocators/freeListAllocator.h>

namespace dory::memory
{
    FreeListAllocator::FreeListAllocator(const std::size_t cellSize, const std::size_t cellsPerChunkCount, BlockAllocator& blockAllocator) noexcept:
        _cellSize(cellSize),
        _cellsPerChunkCount(cellsPerChunkCount),
        _blockAllocator(blockAllocator)
    {
        assert::debug(cellSize > 0, "CellSize must be greater than zero");
        assert::debug(cellsPerChunkCount > 0, "CellsPerChunkCount must be greater than zero");
        //TODO: write assert: cellsPerChunkCount must be multiple of cellsPerPageCount
        assert::debug(_blockAllocator.getPageSize() >= cellSize, "CellSize must fit into a memory page");
        const std::size_t mask = cellSize - 1;
        assert::debug((cellSize & mask) == 0, "CellSize must be a power of 2");
    }

    void* FreeListAllocator::allocate(const std::size_t size) noexcept
    {
    }

    void FreeListAllocator::deallocate(void* ptr) noexcept
    {
    }

    void* FreeListAllocator::allocateNewChunk()
    {

    }
}
