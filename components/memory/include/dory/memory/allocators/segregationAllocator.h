#pragma once

#include "pageAllocator.h"
#include "freeListAllocator.h"

namespace dory::memory
{
    struct MemorySizeClass
    {
        std::size_t size;
        std::size_t slotsPerChunk;
    };

    template<std::size_t SizeClassCount, typename TPageAllocator, typename TLargeObjectAllocator, typename TMemoryBlockNodeAllocator>
    class SegregationAllocator
    {
    private:
        std::size_t _minClass = 0;

        TLargeObjectAllocator& _largeObjectAllocator;
        std::array<FreeListAllocator<TPageAllocator, TMemoryBlockNodeAllocator>, SizeClassCount> _sizeClassAllocators;

    public:
        explicit SegregationAllocator(TPageAllocator& blockAllocator,
            TLargeObjectAllocator& largeObjectAllocator,
            TMemoryBlockNodeAllocator& memoryBlockNodeAllocator,
            std::array<MemorySizeClass, SizeClassCount>& sizeClasses) noexcept:
        _largeObjectAllocator(largeObjectAllocator)
        {
            std::size_t minSize = std::numeric_limits<std::size_t>::max();

            for(size_t i = 0; i < SizeClassCount; ++i)
            {
                std::size_t size = sizeClasses[i].size;

                if(size < minSize)
                {
                    minSize = size;
                }

                new (&_sizeClassAllocators[i]) FreeListAllocator<TPageAllocator, TMemoryBlockNodeAllocator>(
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
                return _sizeClassAllocators[classIndex].allocate();
            }

            return _largeObjectAllocator.allocate(size);
        }

        void deallocate(void* ptr)
        {
            for(size_t i = 0; i < SizeClassCount; ++i)
            {
                auto& allocator = _sizeClassAllocators[i];
                if(allocator.isInRange(ptr))
                {
                    allocator.deallocate(ptr);
                    return;
                }
            }

            if(_largeObjectAllocator.isInRange(ptr))
            {
                 _largeObjectAllocator.deallocate(ptr);
            }

            assert::inhouse(false, "Pointer is not in managed memory of allocator");
        }

    private:
        static constexpr std::size_t log2Ceil(std::size_t x) noexcept
        {
            //following is optimized version of: return x <= 1 ? 0 : std::bit_width(x - 1);

            // bit_width(x - 1) is undefined for x == 0, so we fix that branchlessly
            const std::size_t nonzero = (x != 0);           // 1 if x > 0, 0 if x == 0
            const std::size_t mask = 0 - nonzero;           // all bits set if x > 0, 0 otherwise
            const std::size_t safe = (x - 1) & mask;        // 0 if x == 0, x-1 otherwise
            const std::size_t w = std::bit_width(safe);
            // For x == 0 or 1, the result should be 0
            return w & (0 - (x > 1));                       // zero result if x <= 1
        }

        [[nodiscard]] std::size_t getSizeClassIndex(const std::size_t size) const noexcept
        {
            //following is optimized version of:
            //const std::size_t cls = log2Ceil(size);
            //return (cls < _minClass) ? 0 : (cls - _minClass);

            const std::size_t cls = log2Ceil(size);

            // Compute mask = 0 if cls < _minClass, else all 1s
            const std::size_t diff = cls - _minClass;
            const std::size_t mask = -(cls >= _minClass); // all 1 bits if true, 0 otherwise

            // Return diff & mask → diff when cls >= _minClass, 0 otherwise
            return diff & mask;
        }
    };
}
