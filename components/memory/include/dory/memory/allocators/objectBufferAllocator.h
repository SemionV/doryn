#pragma once

#include <atomic>
#include <mutex>
#include <dory/bitwise/numbers.h>
#include <dory/data-structures/containers/lockfree/spinLock.h>
#include <dory/macros/assert.h>
#include "../resources/memoryBlock.h"
#include "../profilers/iObjectBufferAllocatorProfiler.h"
#include "dory/memory/allocation.h"

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
    class ObjectBufferAllocator
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

        profilers::IObjectBufferAllocatorProfiler* _profiler;
        TPageAllocator& _pageAllocator;
        std::size_t _pagesPerChunkCount {};
        TMemoryBlockNodeAllocator& _memoryBlockNodeAllocator;
        data_structures::containers::lockfree::SpinLockMutex _chunkAllocMutex;

        std::atomic<MemoryBlockNode*> _chunkHead = nullptr;

    public:
        ObjectBufferAllocator(TPageAllocator& pageAllocator,
            TMemoryBlockNodeAllocator& memoryBlockNodeAllocator,
            profilers::IObjectBufferAllocatorProfiler* profiler = nullptr):
            _profiler(profiler),
            _pageAllocator(pageAllocator),
            _memoryBlockNodeAllocator(memoryBlockNodeAllocator)
        {
            auto chunkToPageRatio = chunkSize / _pageAllocator.getPageSize();
            _pagesPerChunkCount = chunkToPageRatio > 0 ? chunkToPageRatio : 1;
        }

        ~ObjectBufferAllocator()
        {
            MemoryBlockNode* node = _chunkHead.load(MemOrder::relaxed);

            while(node != nullptr)
            {
                if(_profiler)
                    _profiler->traceChunkFree(node->memoryBlock);

                if constexpr (std::is_trivially_destructible_v<T>)
                {
                    const auto base = static_cast<std::byte*>(node->memoryBlock.ptr);

                    for(std::size_t i = 0; i < ObjectsPerChunkCount; ++i)
                    {
                        void* ptr = base + slotSize * i;
                        if(_profiler)
                            _profiler->traceFree(ptr, slotSize);

                        std::destroy_at(static_cast<T*>(ptr));
                    }
                }

                if(node->memoryBlock.ptr != nullptr)
                {
                    _pageAllocator.deallocate(node->memoryBlock);
                }

                MemoryBlockNode* prevNode = node->previousNode;
                //TODO: change all allocators to construct/destruct objects automatically(in case of using system allocator destructor is called twice!)
                node->~MemoryBlockNode();
                _memoryBlockNodeAllocator.deallocate(node);
                node = prevNode;
            }
        }

        /*
         * Reserve chunk of memory
         */
        void reserve()
        {
            allocateChunk();
        }

        template<typename... TArgs>
        T* allocate(TArgs... args)
        {
            while(true)
            {
                if(isEmpty())
                {
                    allocateChunk();
                    continue;
                }

                MemoryBlockNode* currentHead = _chunkHead.load(MemOrder::relaxed);
                std::size_t currentIndex = currentHead->index.load(MemOrder::relaxed);
                if(currentIndex >= ObjectsPerChunkCount)
                    continue;

                std::size_t newIndex = currentIndex + 1;
                if(!currentHead->index.compare_exchange_strong(currentIndex, newIndex, MemOrder::release, MemOrder::relaxed))
                    continue;

                //Now thread can use currentIndex slot in currentHead exclusively
                const auto base = static_cast<std::byte*>(currentHead->memoryBlock.ptr);
                void* ptr = base + slotSize * currentIndex;
                ::new (ptr) T(std::forward<TArgs>(args)...);

                if(_profiler)
                    _profiler->traceAllocation(ptr, slotSize);

                return static_cast<T*>(ptr);
            }
        }

        /*
         * Empty means that there are no free objects(slots) left in pool
         */
        bool isEmpty()
        {
            const MemoryBlockNode* currentHead = _chunkHead.load(MemOrder::acquire);
            return currentHead == nullptr || currentHead->index.load(MemOrder::relaxed) >= ObjectsPerChunkCount;
        }

    private:
        void allocateChunk()
        {
            //Prevent multiple threads from allocating chunk of memory(it is cheaper than allocate multiple chunks per thread and choose a winner)
            std::lock_guard lock {_chunkAllocMutex};

            //Check if chunk was allocated by another thread
            MemoryBlockNode* currentHead = _chunkHead.load(MemOrder::acquire);
            if(currentHead != nullptr && currentHead->index.load(MemOrder::relaxed) < ObjectsPerChunkCount)
                return;

            //Allocate node object(bookkeeping) for chunk of memory
            const auto newHead = static_cast<MemoryBlockNode*>(_memoryBlockNodeAllocator.allocate(sizeof(MemoryBlockNode)));
            assert::inhouse(newHead, "Cannot allocate MemoryBlockNode");
            std::construct_at(newHead);

            //Allocate chunk of memory
            auto& memoryBlock = newHead->memoryBlock;
            const ErrorCode errorCode = _pageAllocator.allocate(_pagesPerChunkCount, memoryBlock);
            if(errorCode != ErrorCode::Success)
            {
                assert::inhouse(false, "Cannot allocate memory block");
                return;
            }

            //Set all bytes in chunk to 0
            std::memset(memoryBlock.ptr, 0, memoryBlock.pagesCount * memoryBlock.pageSize);

            //Set new chunk head
            newHead->previousNode = currentHead;
            if(!_chunkHead.compare_exchange_strong(currentHead, newHead, MemOrder::release, MemOrder::relaxed))
            {
                //Some thread has allocated a new chunk(should not happen because of the lock, but just in case)
                //Deallocate chunk and node
                _pageAllocator.deallocate(memoryBlock);
                newHead->~MemoryBlockNode();
                _memoryBlockNodeAllocator.deallocate(newHead);
            }
            else if(_profiler)
            {
                _profiler->traceChunkAllocation(memoryBlock);
            }
        }
    };
}
