#pragma once

#include <cmath>
#include "allocation.h"

namespace dory::memory
{
    enum class ErrorCode
    {
        Success,
        InsufficientSystemMemory,
        OutOfMemory
    };

    class StackAllocator
    {
    private:
        std::size_t _pageSize {};
        std::size_t _pagesCount {};
        std::size_t _totalSize {};
        void* _blockPointer {};
        void* _currentPosition {};

    public:
        explicit StackAllocator(std::size_t estimatedMemoryUsage);

        ErrorCode initialize() noexcept;
        void destroy() noexcept;
        void resetPosition() noexcept;

        template<typename T>
        ErrorCode allocate(const std::size_t size, T** ptr) noexcept
        {
            assert::debug(_blockPointer, "Allocator must be initialized");
            assert::debug(ptr, "Invalid argument");

            const auto address = reinterpret_cast<std::uintptr_t>(_currentPosition);
            const auto alignedAddress = alignAddress(address, alignof(T));
            if(reinterpret_cast<std::uintptr_t>(_blockPointer) + _totalSize < alignedAddress + size)
            {
                assert::inhouse(false, "Insufficient arena block size");
                return ErrorCode::OutOfMemory;
            }

            *ptr = reinterpret_cast<T*>(alignedAddress);

            return ErrorCode::Success;
        }

        template<typename T>
        void deallocate(const T* ptr) noexcept
        {}
    };
}