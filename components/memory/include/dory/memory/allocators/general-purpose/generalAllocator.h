#pragma once
#include <memory>
#include <type_traits>
#include <dory/types.h>
#include <dory/memory/profilers/iAllocatorProfiler.h>

#include "dory/memory/allocation.h"

namespace dory::memory::allocators::general_purpose
{
    template<typename TImplementation>
    class GeneralAllocator
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
            void* ptr = implementation().allocateBytesImpl(label, size, alignment);

            if(ptr && _profiler)
                _profiler->traceBytesAllocation(ptr, size, alignment, label);

            return ptr;
        }

        void deallocateBytes(void* ptr, std::size_t size, std::size_t alignment = defaultAlignment) noexcept
        {
            implementation().deallocateBytesImpl(ptr, size, alignment);

            if(ptr && _profiler)
                _profiler->traceBytesFree(ptr, size, alignment);
        }

        template<typename T, typename... TArgs>
        requires( !std::is_array_v<T> )
        [[nodiscard]] T* allocateObject(const LabelType& label, TArgs&&... args)
        {
            void* ptr = allocateBytes(label, sizeof(T), alignof(T));

            try
            {
                T* obj = std::construct_at(static_cast<T*>(ptr), std::forward<TArgs>(args)...);

                if(_profiler)
                    _profiler->traceObjectAllocation(ptr, sizeof(T), alignof(T), label);

                return obj;
            }
            catch (...)
            {
                deallocateBytes(ptr, sizeof(T), alignof(T));

                //TODO: throw only if compiled with exception support
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

            if(_profiler)
                _profiler->traceObjectFree(ptr, sizeof(T), alignof(T));
        }

        template<typename T>
        [[nodiscard]] T* allocateArray(const std::size_t count, const LabelType& label)
        {
            //TODO: assert size of array has no overflow

            void* ptr = allocateBytes(label, sizeof(T) * count, alignof(T));
            T* obj = static_cast<T*>(ptr);

            std::size_t i = 0;
            try
            {
                for (; i < count; ++i)
                    std::destroy_n(obj, i);
            }
            catch (...)
            {
                std::destroy(obj, obj + i);
                deallocateBytes(ptr, sizeof(T) * count, alignof(T));
                throw;
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
        //TODO: move this methods to a generic parent class for CRTP
        TImplementation& implementation() noexcept
        {
            return static_cast<TImplementation&>(*this);
        }
        const TImplementation& implementation() const noexcept
        {
            return static_cast<const TImplementation&>(*this);
        }
    };
}
