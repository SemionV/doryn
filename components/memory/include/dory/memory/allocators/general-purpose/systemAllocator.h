#pragma once
#include <cstddef>
#include <new>
#include <memory>
#include <type_traits>
#include <utility>

#include <dory/bitwise/numbers.h>
#include <dory/macros/assert.h>

namespace dory::memory::allocators::general_purpose
{
    class SystemAllocator
    {
    public:
        static constexpr std::size_t defaultAlignment = alignof(std::max_align_t);

        [[nodiscard]] void* allocateBytes(std::size_t size, std::size_t alignment = defaultAlignment)
        {
            assert::debug(bitwise::isPowerOfTwo(alignment), "Alignment must be power of two");
            assert::debug(alignment >= alignof(void*), "Alignment must be greater or equal to alignment of a pointer");

            if (size == 0)
                size = 1;

            if (alignment <= defaultAlignment)
                return ::operator new(size);

            return ::operator new(size, static_cast<std::align_val_t>(alignment));
        }

        void deallocateBytes(void* ptr, std::size_t size, std::size_t alignment = defaultAlignment) noexcept
        {
            if (!ptr)
                return;

            if (alignment <= defaultAlignment)
                ::operator delete(ptr);
            else
                ::operator delete(ptr, static_cast<std::align_val_t>(alignment));
        }

        template<typename T, typename... TArgs>
        requires( !std::is_array_v<T> )
        [[nodiscard]] T* allocateObject(TArgs&&... args)
        {
            void* mem = allocateBytes(sizeof(T), alignof(T));
            try
            {
                return std::construct_at(static_cast<T*>(mem), std::forward<TArgs>(args)...);
            }
            catch (...)
            {
                deallocateBytes(mem, sizeof(T), alignof(T));
                throw;
            }
        }

        template<typename T>
        void deallocateObject(T* ptr) noexcept
        {
            if (!ptr)
                return;

            std::destroy_at(ptr);
            deallocateBytes(ptr, sizeof(T), alignof(T));
        }

        template<typename T>
        [[nodiscard]] T* allocateArray(const std::size_t count)
        {
            void* mem = allocateBytes(sizeof(T) * count, alignof(T));
            T* ptr = static_cast<T*>(mem);

            std::size_t i = 0;
            try
            {
                for (; i < count; ++i)
                    std::construct_at(ptr + i);
            }
            catch (...)
            {
                std::destroy(ptr, ptr + i);
                deallocateBytes(mem, sizeof(T) * count, alignof(T));
                throw;
            }

            return ptr;
        }

        template<typename T>
        void deallocateArray(T* ptr, std::size_t count) noexcept
        {
            if (!ptr)
                return;

            std::destroy(ptr, ptr + count);
            deallocateBytes(ptr, sizeof(T) * count, alignof(T));
        }
    };
}