#pragma once

#include <atomic>
#include <dory/bitwise/numbers.h>
#include "../resources/memoryBlock.h"
#include "../profilers/iObjectPoolAllocatorProfiler.h"

namespace dory::memory::allocators
{
    /*
     * Pool of objects(slots), which can be allocated, but cannot be deallocated
     * This allocator is useful for large sets of objects of the same size, which are
     * getting created and stored, but will be disposed all at once.
     * This allocator supports segments(chunks) of memory, which are allocated one after another
     * as soon as segment of is full of allocated objects.
     *
     * The allocator is concurrent
     */
    template<typename T, typename TPageAllocator, typename TMemoryBlockNodeAllocator, std::size_t ObjectsPerChunkCount>
    class ObjectPoolAllocator
    {
    public:
        static constexpr std::size_t slotSize = bitwise::nextPowerOfTwo(sizeof(T));
        static constexpr std::size_t chunkSize = ObjectsPerChunkCount * slotSize;

    private:
        struct MemOrder
        {
            static constexpr auto relaxed = std::memory_order::seq_cst;
            static constexpr auto acquire = std::memory_order::seq_cst;
            static constexpr auto release = std::memory_order::seq_cst;
            static constexpr auto seq_cst = std::memory_order::seq_cst;
        };

        profilers::IObjectPoolAllocatorProfiler* _profiler;
        TPageAllocator& _pageAllocator;
        const std::size_t _pagesPerChunkCount {};
        TMemoryBlockNodeAllocator& _memoryBlockNodeAllocator;
        std::atomic<MemoryBlockNode*> _chunkHead = nullptr;

    public:
        ObjectPoolAllocator(TPageAllocator& pageAllocator,
            TMemoryBlockNodeAllocator& memoryBlockNodeAllocator,
            profilers::IObjectPoolAllocatorProfiler* profiler = nullptr):
            _profiler(profiler),
            _pageAllocator(pageAllocator),
            _memoryBlockNodeAllocator(memoryBlockNodeAllocator)
        {
            auto chunkToPageRatio = chunkSize / _pageAllocator.getPageSize();
            _pagesPerChunkCount = chunkToPageRatio > 0 ? chunkToPageRatio : 1;
        }

        ~ObjectPoolAllocator()
        {
            MemoryBlockNode* node = _chunkHead.load(MemOrder::relaxed);

            while(node != nullptr)
            {
                if(node->memoryBlock.ptr != nullptr)
                {
                    _pageAllocator.deallocate(node->memoryBlock);
                }

                MemoryBlockNode* prevNode = node->previousNode;
                _memoryBlockNodeAllocator.deallocate(node);
                node = prevNode;
            }
        }

        /*
         * Reserve chunks
         */
        void reserve(const std::size_t chunkCount)
        {
            for(std::size_t i = 0; i < chunkCount; ++i)
                allocateChunk();
        }

        T* allocate()
        {

        }

    private:
        void allocateChunk()
        {

        }
    };
}
