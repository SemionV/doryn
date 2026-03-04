#pragma once

#include <atomic>
#include <cstddef>
#include <spdlog/fmt/fmt.h>
#include <dory/macros/assert.h>
#include <dory/data-structures/containers/lockfree/util.h>
#include <dory/memory/profilers/iFreeListAllocProfiler.h>
#include <dory/types.h>
#include <dory/bitwise/numbers.h>
#include <dory/data-structures/containers/lockfree/spinLock.h>

#include <mutex>

namespace dory::memory::allocators::specific
{
    /*
     * Lock-free implementation of a FreeList allocator.
     * It allocates a contiguous chunk of memory, divides it into slots and makes a linked list of the free slots.
     * If all slots in a chunk are used, it allocates next chunk and links the chunk descriptors in a chain(do deallocate them properly on destruction).
     */
    template<typename TPageAllocator, typename TMemoryBlockNodeAllocator, LabelType PageAllocLabel = {}>
    class ObjectPoolAllocator
    {
    private:
        const std::size_t _slotSize {};
        const std::size_t _slotsPerChunkCount {};
        const std::size_t _chunkSize {};
        const std::size_t _pageSize {};
        std::size_t _pageCount {};
        std::size_t _threadsCount {};
        std::size_t _batchSize {};
        TPageAllocator& _pageAllocator; //Allocator of memory chunks
        TMemoryBlockNodeAllocator& _memoryBlockNodeAllocator; //Allocator of MemoryBlock descriptors, wrapped in a Node structure, to make a linked list of all allocated chunks
        profilers::IFreeListAllocProfiler* _profiler;
        alignas(64) std::atomic<void*> _freeListHead; //Pointer to the first node in the free list
        alignas(64) std::atomic<MemoryBlockNode*> _memoryBlockHead; //Pointer to the last node of allocated memory chunks
        alignas(64) std::atomic<MemoryBlockNode*> _pendingBlock; //A newly allocated memory chunk, which is in progress of initialization, each thread, which is seeing it can help to initialize it

        data_structures::containers::lockfree::SpinLockMutex _mutex;

        struct MemOrder
        {
            static constexpr auto relaxed = std::memory_order::relaxed;
            static constexpr auto acquire = std::memory_order::acquire;
            static constexpr auto release = std::memory_order::release;
            static constexpr auto seq_cst = std::memory_order::seq_cst;
        };

    public:
        ObjectPoolAllocator(const std::size_t slotSize, const std::size_t slotsPerChunkCount,
            TPageAllocator& pageAllocator,
            TMemoryBlockNodeAllocator& memoryBlockNodeAllocator,
            profilers::IFreeListAllocProfiler* profiler = nullptr,
            const std::size_t threadsCount = 8) noexcept:
        _slotSize(slotSize),
        _slotsPerChunkCount(slotsPerChunkCount),
        _chunkSize(slotSize * slotsPerChunkCount),
        _pageAllocator(pageAllocator),
        _pageSize(pageAllocator.getBlockSize()),
        _threadsCount(threadsCount),
        _memoryBlockNodeAllocator(memoryBlockNodeAllocator),
        _profiler(profiler)
        {
            assert::debug(slotSize > 0, "SlotSize must be greater than zero");
            assert::debug(slotSize >= sizeof(void*), fmt::format("SlotSize must be at least as big as size of a pointer. SlotSize: {}", slotSize).c_str());
            const std::size_t mask = slotSize - 1;
            assert::debug((slotSize & mask) == 0, "SlotSize must be a power of 2");
            assert::debug(slotsPerChunkCount > 0, "SlotsPerChunkCount must be greater than zero");

            _pageCount = _chunkSize / _pageSize;
            _pageCount = _pageCount == 0 ? 1 : _pageCount;

            std::size_t largeBlockSize = 0;
            std::size_t smallBlockSize = 0;
            if(_chunkSize > _pageSize)
            {
                largeBlockSize = _chunkSize;
                smallBlockSize = _pageSize;
            }
            else
            {
                largeBlockSize = _pageSize;
                smallBlockSize = _chunkSize;
            }
            assert::debug(largeBlockSize % smallBlockSize == 0, fmt::format("Chunk size must be a multiple of page size").c_str());

            assert::debug(_threadsCount > 0, "Threads count must be greater than zero");
            assert::debug(bitwise::isPowerOfTwo(_threadsCount), "Threads count must be a power of 2");

            std::size_t currentThreadsCountLog = bitwise::log2Ceil(_threadsCount);
            while(_batchSize == 0)
            {
                _batchSize = _slotsPerChunkCount >> currentThreadsCountLog;
                --currentThreadsCountLog;
            }

            allocateChunk(); //Allocate the initial memory chunk
        }

        ~ObjectPoolAllocator()
        {
            MemoryBlockNode* node = _memoryBlockHead;

            while(node != nullptr)
            {
                if(node->data != nullptr)
                {
                    _pageAllocator.deallocateBlock(node->data, _pageCount);
                }

                MemoryBlockNode* prevNode = node->previousNode;
                _memoryBlockNodeAllocator.deallocateObject(node);
                node = prevNode;
            }
        }

        void* allocate() noexcept
        {
            void* slotPointer = nullptr;
            void* currentHeadPointer = _freeListHead.load(MemOrder::relaxed);

            while(slotPointer == nullptr)
            {
                if(currentHeadPointer == nullptr)
                {
                    if(!allocateChunk())
                    {
                        //failed to allocate memory block with slots, allocation is not possible
                        return nullptr;
                    }

                    currentHeadPointer = _freeListHead.load(MemOrder::relaxed);

                    //Some threads might be looping while waiting for new slots available, yield is needed to prevent them overheating CPU
                    data_structures::containers::lockfree::cpu_relax();
                    continue;
                }

                void* nextHeadPointer = *static_cast<void* const*>(currentHeadPointer);
                if(_freeListHead.compare_exchange_weak(currentHeadPointer, nextHeadPointer, MemOrder::acquire, MemOrder::relaxed))
                {
                    slotPointer = currentHeadPointer;
                }
            }

            return slotPointer;
        }

        void deallocate(void* ptr) noexcept
        {
            //Write current head pointer value to the deallocated slot and write address of the deallocated slot to the head pointer
            void* headPointer = _freeListHead.load(MemOrder::relaxed);
            *static_cast<void**>(ptr) = headPointer;
            while(!_freeListHead.compare_exchange_weak(headPointer, ptr, MemOrder::release, MemOrder::relaxed))
            {
                *static_cast<void**>(ptr) = headPointer;
            }
        }

        [[nodiscard]] std::size_t getSlotSize() const noexcept
        {
            return _slotSize;
        }

    private:
        MemoryBlockNode* getNewMemoryBlock()
        {
            //Using lock to prevent multiple threads from allocating blocks simultaneously
            std::lock_guard lock { _mutex };

            //Allocate a new MemoryBlockNode as well as a MemoryBlock
            MemoryBlockNode* currentPendingBlock = _pendingBlock.load(MemOrder::acquire);
            if(currentPendingBlock == nullptr)
            {
                auto newPendingBlock = static_cast<MemoryBlockNode*>(_memoryBlockNodeAllocator.template allocateObject<MemoryBlockNode>({}));
                assert::release(newPendingBlock, "Cannot allocate memory block node");

                if(newPendingBlock != nullptr)
                {
                    void* blockMemoryPtr = _pageAllocator.allocateBlock(PageAllocLabel, _pageCount);
                    assert::release(blockMemoryPtr, "Cannot allocate memory block");

                    if(blockMemoryPtr != nullptr)
                    {
                        newPendingBlock->data = blockMemoryPtr;
                        newPendingBlock->index = 0;

                        if(_pendingBlock.compare_exchange_strong(currentPendingBlock, newPendingBlock, MemOrder::release, MemOrder::acquire))
                        {
                            currentPendingBlock = newPendingBlock;

                            //Attach new block to the chain
                            MemoryBlockNode* headNode = _memoryBlockHead.load(MemOrder::relaxed);
                            newPendingBlock->previousNode = headNode;
                            while(!_memoryBlockHead.compare_exchange_weak(headNode, newPendingBlock, MemOrder::release, MemOrder::relaxed))
                            {
                                newPendingBlock->previousNode = headNode;
                            }

                            if(_profiler)
                                _profiler->traceChunkAlloc(newPendingBlock->data, _pageAllocator.getBlockSize() * _pageCount);
                        }
                        else
                        {
                            _pageAllocator.deallocateBlock(blockMemoryPtr, _pageCount);
                            _memoryBlockNodeAllocator.deallocateObject(newPendingBlock);
                        }
                    }
                    else
                    {
                        _memoryBlockNodeAllocator.deallocateObject(newPendingBlock);
                    }
                }
            }

            return currentPendingBlock;
        }

        //Initialize block of slots to free-list(uses batches of slots, number of batches is apprx. equal to number of working threads)
        void feedNewMemoryBlockToFreeList(MemoryBlockNode& memoryBlock)
        {
            std::size_t currentSlot = memoryBlock.index.load(MemOrder::relaxed);
            while(currentSlot < _slotsPerChunkCount)
            {
                std::size_t nextSlot = currentSlot + _batchSize;
                if(!memoryBlock.index.compare_exchange_weak(currentSlot, nextSlot, MemOrder::acquire, MemOrder::relaxed))
                {
                    //try again
                    continue;
                }

                //block of slots is reserved
                const std::size_t endIndex = _slotsPerChunkCount - currentSlot;
                const std::size_t startIndex = _slotsPerChunkCount - currentSlot - _batchSize;


                //Link block of slots together
                for(std::size_t i = startIndex; i < endIndex; ++i)
                {
                    const std::uintptr_t slotAddress = reinterpret_cast<std::uintptr_t>(memoryBlock.data) + _slotSize * i;

                    if(i != endIndex - 1)
                    {
                        const std::uintptr_t nextSlotAddress = reinterpret_cast<std::uintptr_t>(memoryBlock.data) + _slotSize * (i + 1);
                        *reinterpret_cast<std::uintptr_t*>(slotAddress) = nextSlotAddress;
                    }
                    else
                    {
                        *reinterpret_cast<void**>(slotAddress) = nullptr;
                    }
                }

                //Feed block of slots into free-list
                const std::uintptr_t firstSlotAddress = reinterpret_cast<std::uintptr_t>(memoryBlock.data) + _slotSize * startIndex;
                const std::uintptr_t lastSlotAddress = reinterpret_cast<std::uintptr_t>(memoryBlock.data) + _slotSize * (endIndex - 1);
                void* freeListHead = _freeListHead.load(MemOrder::relaxed);
                *reinterpret_cast<void**>(lastSlotAddress) = freeListHead;
                while(!_freeListHead.compare_exchange_weak(freeListHead, reinterpret_cast<void*>(firstSlotAddress), MemOrder::release, MemOrder::relaxed))
                {
                    *reinterpret_cast<void**>(lastSlotAddress) = freeListHead;
                }

                currentSlot = memoryBlock.index.load(MemOrder::relaxed);
            }
        }

        bool allocateChunk()
        {
            MemoryBlockNode* newMemoryBlock = getNewMemoryBlock();
            assert::release(newMemoryBlock, "Cannot get new memory block");
            if(!newMemoryBlock)
                return false;

            feedNewMemoryBlockToFreeList(*newMemoryBlock);

            //One of the threads will succeed
            _pendingBlock.compare_exchange_strong(newMemoryBlock, nullptr, MemOrder::release, MemOrder::acquire);

            return true;
        }
    };
}