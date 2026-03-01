#pragma once

#include <atomic>
#include <cstddef>
#include <spdlog/fmt/fmt.h>
#include <dory/macros/assert.h>
#include <dory/data-structures/containers/lockfree/util.h>
#include <dory/memory/profilers/iFreeListAllocProfiler.h>
#include <dory/types.h>
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
        const std::size_t _slotSize;
        const std::size_t _slotsPerChunkCount;
        const std::size_t _chunkSize;
        const std::size_t _pageSize;
        std::size_t _pageCount;
        TPageAllocator& _pageAllocator; //Allocator of memory chunks
        TMemoryBlockNodeAllocator& _memoryBlockNodeAllocator; //Allocator of MemoryBlock descriptors, wrapped in a Node structure, to make a linked list of all allocated chunks
        profilers::IFreeListAllocProfiler* _profiler;
        alignas(64) std::atomic<void*> _freeListHead; //Pointer to the first node in the free list
        alignas(64) std::atomic<MemoryBlockNode*> _memoryBlockHead; //Pointer to the last node of allocated memory chunks
        alignas(64) std::atomic<MemoryBlockNode*> _pendingBlock; //A newly allocated memory chunk, which is in progress of initialization, each thread, which is seeing it can help to initialize it

        data_structures::containers::lockfree::SpinLockMutex _mutex;

        struct MemOrder
        {
            static constexpr auto relaxed = std::memory_order::seq_cst;
            static constexpr auto acquire = std::memory_order::seq_cst;
            static constexpr auto release = std::memory_order::seq_cst;
            static constexpr auto seq_cst = std::memory_order::seq_cst;
        };

    public:
        ObjectPoolAllocator(const std::size_t slotSize, const std::size_t slotsPerChunkCount,
            TPageAllocator& pageAllocator,
            TMemoryBlockNodeAllocator& memoryBlockNodeAllocator,
            profilers::IFreeListAllocProfiler* profiler = nullptr) noexcept:
        _slotSize(slotSize),
        _slotsPerChunkCount(slotsPerChunkCount),
        _chunkSize(slotSize * slotsPerChunkCount),
        _pageAllocator(pageAllocator),
        _pageSize(pageAllocator.getBlockSize()),
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

            allocateChunk(); //Allocate the initial memory chunk
        }

        ~ObjectPoolAllocator()
        {
            MemoryBlockNode* node = _memoryBlockHead;

            while(node != nullptr)
            {
                if(node->memoryBlock.ptr != nullptr)
                {
                    _pageAllocator.deallocateBlock(node->memoryBlock.ptr, _pageCount);
                }

                MemoryBlockNode* prevNode = node->previousNode;
                _memoryBlockNodeAllocator.template deallocateObject<MemoryBlockNode>(node);
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
            assert::inhouse(isInRange(ptr), "Pointer does not belong to the managed memory of the allocator");

            //Write current head pointer value to the deallocated slot and write address of the deallocated slot to the head pointer
            void* headPointer = _freeListHead.load(MemOrder::relaxed);
            *static_cast<void**>(ptr) = headPointer;
            while(!_freeListHead.compare_exchange_weak(headPointer, ptr, MemOrder::release, MemOrder::relaxed))
            {
                *static_cast<void**>(ptr) = headPointer;
            }
        }

        bool isInRange(void* ptr) const
        {
            MemoryBlockNode* node = _memoryBlockHead;
            bool isInRange = false;

            //Check that pointer is within one of the allocated memory chunks
            while(node != nullptr)
            {
                MemoryBlock& memoryBlock = node->memoryBlock;

                if(node->memoryBlock.ptr != nullptr)
                {
                    const auto chunkStartAddress = reinterpret_cast<uintptr_t>(memoryBlock.ptr);
                    const std::uintptr_t chunkEndAddress = chunkStartAddress + memoryBlock.pageSize * memoryBlock.pagesCount;
                    const auto address = reinterpret_cast<uintptr_t>(ptr);
                    if(address >= chunkStartAddress && address <= chunkEndAddress - sizeof(void*))
                    {
                        isInRange = true;
                        break;
                    }
                }

                node = node->previousNode;
            }

            return isInRange;
        }

        [[nodiscard]] std::size_t getSlotSize() const noexcept
        {
            return _slotSize;
        }

    private:
        bool allocateChunk()
        {
            std::lock_guard lock { _mutex };

            //Allocate a new MemoryBlockNode as well as a MemoryBlock
            MemoryBlockNode* pendingBlock = _pendingBlock.load(MemOrder::acquire);
            if(pendingBlock == nullptr)
            {
                auto newBlock = static_cast<MemoryBlockNode*>(_memoryBlockNodeAllocator.template allocateObject<MemoryBlockNode>({}));
                assert::inhouse(newBlock, "Cannot allocate MemoryBlockNode");
                std::construct_at(newBlock);

                if(newBlock == nullptr)
                {
                    assert::release(false, "Cannot allocate memory block node");
                    return false;
                }

                newBlock->memoryBlock.ptr = _pageAllocator.allocateBlock(PageAllocLabel, _pageCount);
                newBlock->memoryBlock.pagesCount = _pageCount;
                newBlock->memoryBlock.pageSize = _pageAllocator.getBlockSize();
                if(newBlock->memoryBlock.ptr == nullptr)
                {
                    assert::release(false, "Cannot allocate memory block");
                    return false;
                }

                if(_pendingBlock.compare_exchange_strong(pendingBlock, newBlock, MemOrder::release, MemOrder::acquire))
                {
                    pendingBlock = newBlock;
                    if(_profiler)
                        _profiler->traceChunkAlloc(newBlock->memoryBlock);
                }
                else
                {
                    _pageAllocator.deallocateBlock(newBlock->memoryBlock.ptr, _pageCount);
                    newBlock->~MemoryBlockNode();
                    _memoryBlockNodeAllocator.template deallocateObject<MemoryBlockNode>(newBlock);
                }
            }

            //Initialize linked list in the memory block
            MemoryBlock& memoryBlock = pendingBlock->memoryBlock;
            std::atomic<std::size_t>& initializationIndex = pendingBlock->index;
            std::size_t currentSlot = initializationIndex.load(MemOrder::relaxed);
            while(currentSlot < _slotsPerChunkCount)
            {
                std::size_t nextSlot = currentSlot + 1;
                if(!initializationIndex.compare_exchange_strong(currentSlot, nextSlot, MemOrder::relaxed, MemOrder::relaxed))
                {
                    continue;
                }

                const std::uintptr_t slotAddress = reinterpret_cast<std::uintptr_t>(memoryBlock.ptr) + _slotSize * currentSlot;

                if(currentSlot != _slotsPerChunkCount - 1)
                {
                    const std::uintptr_t nextSlotAddress = reinterpret_cast<std::uintptr_t>(memoryBlock.ptr) + _slotSize * (currentSlot + 1);
                    *reinterpret_cast<std::uintptr_t*>(slotAddress) = nextSlotAddress;
                }
                else
                {
                    *reinterpret_cast<void**>(slotAddress) = nullptr;
                }

                currentSlot = initializationIndex.load(MemOrder::relaxed);
            }

            //Update bookkeeping references
            initializationIndex.compare_exchange_strong(currentSlot, 0, MemOrder::relaxed, MemOrder::relaxed);

            if(_profiler)
                _profiler->traceChunkInitialized(memoryBlock);

            if(_pendingBlock.compare_exchange_strong(pendingBlock, nullptr, MemOrder::release, MemOrder::acquire))
            {
                MemoryBlockNode* headNode = _memoryBlockHead.load(MemOrder::relaxed);
                pendingBlock->previousNode = headNode;
                while(!_memoryBlockHead.compare_exchange_weak(headNode, pendingBlock, MemOrder::release, MemOrder::relaxed))
                {
                    pendingBlock->previousNode = headNode;
                }

                if(_profiler)
                    _profiler->traceChunkChained(*pendingBlock);

                //Another thread can free some slots and put them on free list, this is why _freeListHead could be not nullptr, but some valid pointer to a slot
                void* freeListHead = _freeListHead.load(MemOrder::relaxed);
                const std::uintptr_t slotAddress = reinterpret_cast<std::uintptr_t>(memoryBlock.ptr) + _slotSize * (_slotsPerChunkCount - 1);
                *reinterpret_cast<void**>(slotAddress) = freeListHead;
                while(!_freeListHead.compare_exchange_weak(freeListHead, memoryBlock.ptr, MemOrder::release, MemOrder::relaxed))
                {
                    *reinterpret_cast<void**>(slotAddress) = freeListHead;
                }
            }
            else
            {
                //TODO: possible starvation
                while(_freeListHead.load(MemOrder::relaxed) == nullptr)
                {
                    data_structures::containers::lockfree::cpu_relax();
                }
            }

            return true;
        }
    };
}