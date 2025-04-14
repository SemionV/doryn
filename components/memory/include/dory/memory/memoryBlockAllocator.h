#pragma once

#include <dory/macros/assert.h>
#include "allocation.h"

namespace dory::memory
{
    class MemoryBlockAllocator
    {
        std::size_t _pageSize {};
        const std::size_t _pagesCount {};
        void* _pointer {};
        ErrorCode _errorCode  { ErrorCode::Success };

    public:
        explicit MemoryBlockAllocator(std::size_t pagesCount) noexcept;
        ~MemoryBlockAllocator() noexcept;

        [[nodiscard]] void* get() const noexcept;
        [[nodiscard]] ErrorCode getErrorCode() const noexcept;
    };
}
