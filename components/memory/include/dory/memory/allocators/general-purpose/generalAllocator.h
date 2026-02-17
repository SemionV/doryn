#pragma once
#include <memory>
#include <type_traits>
#include <dory/types.h>
#include <dory/base.h>
#include <dory/memory/profilers/iAllocatorProfiler.h>
#include <dory/macros/assert.h>

#include "dory/memory/allocation.h"

namespace dory::memory::allocators::general_purpose
{
    template<typename TImplementation>
    class GeneralAllocator: public Base<TImplementation>
    {
    private:
        profilers::IAllocatorProfiler* _profiler {};

    protected:
        static constexpr std::size_t defaultAlignment = alignof(std::max_align_t);

    public:
        explicit GeneralAllocator(profilers::IAllocatorProfiler* profiler) noexcept:
            _profiler(profiler)
        {}

        [[nodiscard]] void* allocateBytes(const LabelType& label, std::size_t size, std::size_t alignment = defaultAlignment)
        {
            void* ptr = this->implRef().allocateBytesImpl(label, size, alignment);

            if(ptr && _profiler)
                _profiler->traceBytesAllocation(ptr, size, alignment, label);

            return ptr;
        }

        void deallocateBytes(void* ptr, std::size_t size, std::size_t alignment = defaultAlignment) noexcept
        {
            this->implRef().deallocateBytesImpl(ptr, size, alignment);

            if(ptr && _profiler)
                _profiler->traceBytesFree(ptr, size, alignment);
        }

        template<typename T, typename... TArgs>
        requires( !std::is_array_v<T> )
        [[nodiscard]] T* allocateObject(const LabelType& label, TArgs&&... args)
        {
            void* ptr = allocateBytes(label, sizeof(T), alignof(T));

            T* obj = constructAt(static_cast<T*>(ptr), std::forward<TArgs>(args)...);

            if(!obj)
            {
                deallocateBytes(ptr, sizeof(T), alignof(T));
                return nullptr;
            }

            if(_profiler)
                _profiler->traceObjectAllocation(ptr, sizeof(T), alignof(T), label);

            return obj;
        }

        template<typename T>
        void deallocateObject(T* ptr) noexcept
        {
            if (!ptr)
                return;

            std::destroy_at(ptr);
            deallocateBytes(ptr, sizeof(T), alignof(T));

            if(_profiler)
                _profiler->traceObjectFree(ptr, sizeof(T), alignof(T));
        }

        template<typename T, typename... TArgs>
        [[nodiscard]] T* allocateArray(const LabelType& label, const std::size_t count, TArgs&& ...args)
        {
            constexpr std::size_t maxCount = std::numeric_limits<std::size_t>::max() / sizeof(T);
            assert::debug(count <= maxCount, "Array size overflow");

            const std::size_t arraySize = sizeof(T) * count;

            void* ptr = allocateBytes(label, arraySize, alignof(T));
            if (!ptr)
                return nullptr;

            T* obj = static_cast<T*>(ptr);

            for (std::size_t i = 0; i < count; ++i)
            {
                T* location = constructAt(obj + i, std::forward<TArgs>(args)...);
                if(!location)
                {
                    std::destroy_n(obj, i);
                    deallocateBytes(ptr, arraySize, alignof(T));

                    return nullptr;
                }
            }

            if(_profiler)
                _profiler->traceArrayAllocation(obj, count, sizeof(T), alignof(T), label);

            return obj;
        }

        template<typename T>
        void deallocateArray(T* ptr, std::size_t count) noexcept
        {
            if (!ptr)
                return;

            std::destroy(ptr, ptr + count);

            deallocateBytes(ptr, sizeof(T) * count, alignof(T));

            if(_profiler)
                _profiler->traceArrayFree(ptr, count, sizeof(T), alignof(T));
        }

    private:
        template<typename T, typename... TArgs>
        T* constructAt(T* ptr, TArgs&& ...args)
            noexcept(
                (sizeof...(TArgs) == 0 && std::is_nothrow_default_constructible_v<T>) ||
                (sizeof...(TArgs) != 0 && std::is_nothrow_constructible_v<T, TArgs...>)
            )
        {
            if constexpr (sizeof...(TArgs) > 0)
            {
                return std::construct_at(ptr, std::forward<TArgs>(args)...);
            }
            else
            {
                ::new(static_cast<void*>(ptr)) T{};
                return ptr;
            }
        }
    };
}
