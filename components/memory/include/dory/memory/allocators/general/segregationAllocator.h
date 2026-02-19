#pragma once

#include "dory/bitwise/numbers.h"
#include <dory/memory/profilers/iAllocatorProfiler.h>
#include <dory/memory/allocators/general/generalAllocator.h>
#include <dory/memory/allocators/specific/objectPoolAllocator.h>

namespace dory::memory::allocators::general
{
    struct MemorySizeClass
    {
        std::size_t size;
        std::size_t slotsPerChunk;
    };

    template<std::size_t SizeClassCount, typename TPageAllocator, typename TLargeObjectAllocator, typename TMemoryBlockNodeAllocator>
    class SegregationAllocator: public GeneralAllocator<SegregationAllocator<SizeClassCount, TPageAllocator, TLargeObjectAllocator, TMemoryBlockNodeAllocator>>
    {
    private:
        using BaseType = GeneralAllocator<SegregationAllocator>;
        using FreeListAllocatorType = specific::ObjectPoolAllocator<TPageAllocator, TMemoryBlockNodeAllocator>;

        std::size_t _minClass = 0;

        TLargeObjectAllocator& _largeObjectAllocator;

        alignas(FreeListAllocatorType) std::byte _sizeClassStorage[SizeClassCount * sizeof(FreeListAllocatorType)];

        FreeListAllocatorType* sizeClassAllocators() noexcept
        {
            return reinterpret_cast<FreeListAllocatorType*>(_sizeClassStorage);
        }

    public:
        explicit SegregationAllocator(TPageAllocator& blockAllocator,
            TLargeObjectAllocator& largeObjectAllocator,
            TMemoryBlockNodeAllocator& memoryBlockNodeAllocator,
            profilers::IAllocatorProfiler* profiler,
            std::array<MemorySizeClass, SizeClassCount>& sizeClasses) noexcept:
        BaseType(profiler),
        _largeObjectAllocator(largeObjectAllocator)
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

        [[nodiscard]] void* allocateBytesImpl(const LabelType& label, std::size_t size, std::size_t alignment)
        {
            const std::size_t classIndex = getSizeClassIndex(size);
            if(classIndex < SizeClassCount)
            {
                auto& allocator = sizeClassAllocators()[classIndex];
                void* ptr = allocator.allocate();

                return ptr;
            }

            void* ptr = _largeObjectAllocator.allocateBytes(label, size, alignment);
            return ptr;
        }

        void deallocateBytesImpl(void* ptr, std::size_t size, std::size_t alignment) noexcept
        {
            const std::size_t classIndex = getSizeClassIndex(size);
            if(classIndex < SizeClassCount)
            {
                auto& allocator = sizeClassAllocators()[classIndex];
                allocator.deallocate(ptr);

                return;
            }

            _largeObjectAllocator.deallocateBytes(ptr, size, alignment);
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
