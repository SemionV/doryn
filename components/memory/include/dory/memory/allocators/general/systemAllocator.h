#pragma once
#include <cstddef>
#include <new>
#include <dory/bitwise/numbers.h>
#include <dory/macros/assert.h>
#include <dory/types.h>

#include "generalAllocator.h"

namespace dory::memory::allocators::general
{
    class SystemAllocator: public GeneralAllocator<SystemAllocator>
    {
        friend class GeneralAllocator;

    public:
        static constexpr std::size_t defaultAlignment = alignof(std::max_align_t);

        explicit SystemAllocator(profilers::IAllocatorProfiler* profiler)
            : GeneralAllocator(profiler)
        {}

    private:

        [[nodiscard]] void* allocateBytesImpl(const LabelType& label, std::size_t size, std::size_t alignment = defaultAlignment)
        {
            assert::debug(bitwise::isPowerOfTwo(alignment), "Alignment must be power of two");
            assert::debug(alignment >= alignof(void*), "Alignment must be greater or equal to alignment of a pointer");

            if (size == 0)
                size = 1;

            if (alignment <= defaultAlignment)
                return ::operator new(size);

            return ::operator new(size, static_cast<std::align_val_t>(alignment));
        }

        void deallocateBytesImpl(void* ptr, std::size_t size, std::size_t alignment = defaultAlignment) noexcept
        {
            if (!ptr)
                return;

            if (alignment <= defaultAlignment)
                ::operator delete(ptr);
            else
                ::operator delete(ptr, static_cast<std::align_val_t>(alignment));
        }
    };
}