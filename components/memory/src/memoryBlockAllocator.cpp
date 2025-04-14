#include <dory/memory/memoryBlockAllocator.h>

namespace dory::memory
{
    MemoryBlockAllocator::MemoryBlockAllocator(const std::size_t pagesCount) noexcept:
        _pagesCount(pagesCount)
    {
        assert::debug(_pagesCount, "Pages count cannot be zero");

        _pageSize = getSystemMemoryPageSize();
        _pointer = reserveMemoryPages(_pageSize, _pagesCount);

        if(!_pointer)
        {
            _errorCode = ErrorCode::OutOfMemory;
        }

        commitMemoryPages(_pointer, _pageSize, _pagesCount);
    }

    MemoryBlockAllocator::~MemoryBlockAllocator() noexcept
    {
        if(_pointer)
        {
            releaseMemoryPages(_pointer, _pageSize, _pagesCount);
            _pointer = nullptr;
        }
    }

    void* MemoryBlockAllocator::get() const noexcept
    {
        return _pointer;
    }

    ErrorCode MemoryBlockAllocator::getErrorCode() const noexcept
    {
        return _errorCode;
    }
}
