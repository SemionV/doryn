#include <dory/memory/allocators/blockAllocator.h>

namespace dory::memory
{
    BlockAllocator::BlockAllocator(std::size_t pageSize) noexcept:
        _pageSize(pageSize)
    {
        assert::debug(pageSize, "Page size cannot be zero");
    }

    ErrorCode BlockAllocator::allocate(const std::size_t pagesCount, MemoryBlock& memoryBlock) const noexcept
    {
        assert::debug(!memoryBlock.ptr, "Using an existing memory block for allocation");

        void* pointer = reserveMemoryPages(_pageSize, pagesCount);
        if(!pointer)
        {
            return ErrorCode::OutOfMemory;
        }

        memoryBlock.ptr = pointer;
        memoryBlock.pagesCount = pagesCount;
        memoryBlock.pageSize = _pageSize;
        memoryBlock.commitedPagesCount = 0;

        return ErrorCode::Success;
    }

    void BlockAllocator::deallocate(const MemoryBlock& memoryBlock) const noexcept
    {
        assert::debug(memoryBlock.ptr, "Trying to deallocate an invalid memory block");

        releaseMemoryPages(memoryBlock.ptr, _pageSize, memoryBlock.pagesCount);
    }
}
