#include <dory/memory/allocators/freeListAllocator.h>

namespace dory::memory
{
    FreeListAllocator::FreeListAllocator(const std::size_t cellSize, const std::size_t cellsPerChunkCount, BlockAllocator& blockAllocator) noexcept:
        _cellSize(cellSize),
        _cellsPerChunkCount(cellsPerChunkCount),
        _blockAllocator(blockAllocator)
    {
        assert::debug(cellSize > 0, "CellSize must be greater than zero");
        assert::debug(cellSize > sizeof(void*), "CellSize must be at least as big as size of a pointer");
        assert::debug(cellsPerChunkCount > 0, "CellsPerChunkCount must be greater than zero");
        //TODO: write assert: cellsPerChunkCount must be multiple of cellsPerPageCount
        assert::debug(_blockAllocator.getPageSize() >= cellSize, "CellSize must fit into a memory page");
        const std::size_t mask = cellSize - 1;
        assert::debug((cellSize & mask) == 0, "CellSize must be a power of 2");

        allocateChunk();
    }

    FreeListAllocator::~FreeListAllocator()
    {
        if(_memoryBlock.ptr != nullptr)
        {
            _blockAllocator.deallocate(_memoryBlock);
        }
    }

    void* FreeListAllocator::allocate(const std::size_t size) noexcept
    {
        assert::debug(size <= _cellSize, "Requested memory allocation is larger than a memory cell");

        while(true)
        {
            void* headPointer = _freeListHead.load(std::memory_order::relaxed);

            if(headPointer ==nullptr) [[unlikely]]
            {
                assert::release(false, "Out of memory");
                return nullptr;
            }

            void* nextCellPointer = *static_cast<void* const*>(headPointer);
            if(_freeListHead.compare_exchange_weak(headPointer, nextCellPointer, std::memory_order::acquire, std::memory_order::relaxed))
            {
                return headPointer;
            }
        }
    }

    void FreeListAllocator::deallocate(void* ptr) noexcept
    {
    }

    void FreeListAllocator::allocateChunk()
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
}
