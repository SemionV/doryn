#include <dory/memory/allocators/pageAllocator.h>

namespace dory::memory
{
    PageAllocator::PageAllocator(std::size_t pageSize) noexcept:
        _pageSize(pageSize)
    {
        assert::debug(pageSize, "Page size cannot be zero");
    }

    ErrorCode PageAllocator::allocate(const std::size_t pagesCount, MemoryBlock& memoryBlock) const noexcept
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

    void PageAllocator::deallocate(const MemoryBlock& memoryBlock) const noexcept
    {
        assert::debug(memoryBlock.ptr, "Trying to deallocate an invalid memory block");

        releaseMemoryPages(memoryBlock.ptr, _pageSize, memoryBlock.pagesCount);
    }

    std::size_t PageAllocator::getPageSize() const noexcept
    {
        return _pageSize;
    }

    void PageAllocator::commitPages(const MemoryBlock& memoryBlock, const std::size_t pagesCount)
    {
        assert::debug(memoryBlock.commitedPagesCount + pagesCount <= memoryBlock.pagesCount, "Invalid pages count");
        commitMemoryPages(memoryBlock.ptr, memoryBlock.pageSize, pagesCount);
    }
}
