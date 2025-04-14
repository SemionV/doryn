#include <dory/memory/stackAllocator.h>

namespace dory::memory
{
    StackAllocator::StackAllocator(const std::size_t estimatedMemoryUsage) noexcept
    {
        _pageSize = getSystemMemoryPageSize();
        assert::inhouse(_pageSize > 0, "Page size cannot be zero or negative");
        assert::inhouse(std::numeric_limits<std::size_t>::max() - estimatedMemoryUsage >= _pageSize - 1, "Requested memory block cannot bit larger than max number of bytes");
        _pagesCount = (estimatedMemoryUsage + _pageSize - 1) / _pageSize;
        _totalSize = _pageSize * _pagesCount;
    }

    ErrorCode StackAllocator::initialize() noexcept
    {
        assert::debug(!_blockPointer, "Repeated initialization attempt");
        assert::debug(!_currentPosition, "Repeated initialization attempt");

        void* ptr = reserveMemoryPages(_pageSize, _pagesCount);
        if(!ptr)
        {
            return ErrorCode::OutOfMemory;
        }

        _blockPointer = ptr;
        _currentPosition = ptr;

        commitMemoryPages(ptr, _pageSize, _pagesCount);

        return ErrorCode::Success;
    }

    void StackAllocator::destroy() noexcept
    {
        assert::debug(_blockPointer, "Destroying uninitialized allocator");
        assert::debug(_currentPosition, "Destroying uninitialized allocator");

        releaseMemoryPages(_blockPointer, _pageSize, _pagesCount);

        _blockPointer = nullptr;
        resetPosition();
    }

    void StackAllocator::resetPosition() noexcept
    {
        _currentPosition = nullptr;
    }
}
