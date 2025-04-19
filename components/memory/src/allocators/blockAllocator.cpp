#include <dory/memory/allocators/blockAllocator.h>

namespace dory::memory
{
    BlockAllocator::BlockAllocator(std::size_t pageSize) noexcept:
        _pageSize(pageSize)
    {
        assert::debug(pageSize, "Page size cannot be zero");
    }

    ErrorCode BlockAllocator::allocate(const std::size_t size, MemoryBlock& memoryBlock) const noexcept
    {
        assert::debug(!memoryBlock.ptr, "Using an existing memory< block for an allocation");

        const auto pagesCount = (size + _pageSize - 1) / _pageSize;
        void* pointer = reserveMemoryPages(_pageSize, pagesCount);
        if(!pointer)
        {
            return ErrorCode::OutOfMemory;
        }

        //TODO: let the user commit the pages manually, this might be needed in pool allocators
        commitMemoryPages(pointer, _pageSize, pagesCount);

        memoryBlock.ptr = pointer;
        memoryBlock.size = size;

        return ErrorCode::Success;
    }

    void BlockAllocator::deallocate(const MemoryBlock& memoryBlock) const noexcept
    {
        assert::debug(memoryBlock.ptr, "Trying to deallocate an invalid memory block");

        const auto pagesCount = (memoryBlock.size + _pageSize - 1) / _pageSize;
        releaseMemoryPages(memoryBlock.ptr, _pageSize, pagesCount);
    }
}
