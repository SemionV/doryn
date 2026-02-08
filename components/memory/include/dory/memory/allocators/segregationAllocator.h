#pragma once

#include <bit>
#include "pageAllocator.h"
#include "freeListAllocator.h"
#include <dory/bitwise/numbers.h>

namespace dory::memory
{
    struct MemorySizeClass
    {
        std::size_t size;
        std::size_t slotsPerChunk;
    };

    template<std::size_t SizeClassCount, typename TPageAllocator, typename TLargeObjectAllocator, typename TMemoryBlockNodeAllocator, typename TProfiler>
    class SegregationAllocator
    {
    private:
        using FreeListAllocatorType = FreeListAllocator<TPageAllocator, TMemoryBlockNodeAllocator>;

        std::size_t _minClass = 0;
        const char* _allocatorName;

        TProfiler& _profiler;
        TLargeObjectAllocator& _largeObjectAllocator;

        alignas(FreeListAllocatorType) std::byte _sizeClassStorage[SizeClassCount * sizeof(FreeListAllocatorType)];

        FreeListAllocatorType* sizeClassAllocators() noexcept
        {
            return reinterpret_cast<FreeListAllocatorType*>(_sizeClassStorage);
        }

    public:
        explicit SegregationAllocator(const char * allocatorName,
            TPageAllocator& blockAllocator,
            TLargeObjectAllocator& largeObjectAllocator,
            TMemoryBlockNodeAllocator& memoryBlockNodeAllocator,
            TProfiler& profiler,
            std::array<MemorySizeClass, SizeClassCount>& sizeClasses) noexcept:
        _largeObjectAllocator(largeObjectAllocator),
        _allocatorName(allocatorName),
        _profiler(profiler)
        {
            std::size_t minSize = std::numeric_limits<std::size_t>::max();
            auto allocators = sizeClassAllocators();

            for(size_t i = 0; i < SizeClassCount; ++i)
            {
                std::size_t size = sizeClasses[i].size;

                const auto& cls = sizeClasses[i];
                minSize = std::min(minSize, size);

                new (&allocators[i]) FreeListAllocatorType(
                    size,
                    sizeClasses[i].slotsPerChunk,
                    blockAllocator,
                    memoryBlockNodeAllocator
                );
            }

            if(minSize > 1)
            {
                _minClass = std::bit_width(minSize - 1);
            }
        }

        void* allocate(const std::size_t size)
        {
            const std::size_t classIndex = getSizeClassIndex(size);
            if(classIndex < SizeClassCount)
            {
                auto& allocator = sizeClassAllocators()[classIndex];
                void* ptr = allocator.allocate();

                _profiler.traceSlotAlloc(ptr, size, allocator.getSlotSize(), classIndex);

                return ptr;
            }

            void* ptr = _largeObjectAllocator.allocate(size);
            _profiler.traceLargeAlloc(ptr, size);
            return ptr;
        }

        template<typename T, typename... TArgs>
        T* allocate(TArgs&&... args)
        {
            void* mem = allocate(sizeof(T));
            return new (mem) T(std::forward<TArgs>(args)...);
        }

        void deallocate(void* ptr)
        {
            if(ptr != nullptr)
            {
                for(size_t i = 0; i < SizeClassCount; ++i)
                {
                    auto& allocator = sizeClassAllocators()[i];
                    if(allocator.isInRange(ptr))
                    {
                        allocator.deallocate(ptr);
                        _profiler.traceSlotFree(ptr, allocator.getSlotSize(), i);

                        return;
                    }
                }

                if(_largeObjectAllocator.isInRange(ptr))
                {
                    _largeObjectAllocator.deallocate(ptr);
                    _profiler.traceLargeFree(ptr, 0);
                }
                else
                {
                    assert::inhouse(false, "Pointer is not in managed memory of allocator");
                }
            }
        }

        void deallocate(void* ptr, const std::size_t size)
        {
            const std::size_t classIndex = getSizeClassIndex(size);
            if(classIndex < SizeClassCount)
            {
                auto& allocator = sizeClassAllocators()[classIndex];
                allocator.deallocate(ptr);

                _profiler.traceSlotFree(ptr, allocator.getSlotSize(), classIndex);
                return;
            }

            if(_largeObjectAllocator.isInRange(ptr))
            {
                _largeObjectAllocator.deallocate(ptr);
                _profiler.traceLargeFree(ptr, size);
            }
            else
            {
                assert::inhouse(false, "Pointer is not in managed memory of allocator");
            }
        }

        template<typename T>
        void deallocateType(T* ptr)
        {
            if(ptr)
            {
                ptr->~T();
            }

            deallocate(ptr, sizeof(T));
        }

    private:
        [[nodiscard]] std::size_t getSizeClassIndex(const std::size_t size) const noexcept
        {
            //following is optimized version of:
            //const std::size_t cls = log2Ceil(size);
            //return (cls < _minClass) ? 0 : (cls - _minClass);

            const std::size_t cls = bitwise::log2Ceil(size);

            // Compute mask = 0 if cls < _minClass, else all 1s
            const std::size_t diff = cls - _minClass;
            const std::size_t mask = -(cls >= _minClass); // all 1 bits if true, 0 otherwise

            // Return diff & mask → diff when cls >= _minClass, 0 otherwise
            return diff & mask;
        }
    };
}
