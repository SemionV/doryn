#pragma once
#include <atomic>
#include <cstddef>
#include "pageAllocator.h"

namespace dory::memory
{
    /*
     * Lock-free implementation of a FreeList allocator.
     * It allocates a contiguous chunk of memory, divides it into slots and makes a linked list of the free slots.
     * If all slots in a chunk are used, it allocates next chunk and links the chunk descriptors in a chain(do deallocate them properly on destruction).
     */
    template<typename TPageAllocator, typename TMemoryBlockNodeAllocator>
    class FreeListAllocator
    {
    private:
        const std::size_t _slotSize;
        const std::size_t _slotsPerChunkCount;
        TPageAllocator& _pageAllocator; //Allocator of memory chunks
        TMemoryBlockNodeAllocator& _memoryBlockNodeAllocator; //Allocator of MemoryBlock descriptors, wrapped in a Node structure, to make a linked list of all allocated chunks
        alignas(64) std::atomic<void*> _freeListHead; //Pointer to the first node in the free list
        alignas(64) std::atomic<MemoryBlockNode*> _memoryBlockHead; //Pointer to the last node of allocated memory chunks
        alignas(64) std::atomic<MemoryBlockNode*> _pendingBlock; //A newly allocated memory chunk, which is in progress of initialization, ech thread, which is seeing it can help to initialize it
        alignas(64) std::atomic<std::size_t> _currentSlotInitialization; //Index of a slot in _pendingBlock, which currently on initialization, each thread can peek one

    public:
        FreeListAllocator(const std::size_t slotSize, const std::size_t slotsPerChunkCount,
            TPageAllocator& pageAllocator,
            TMemoryBlockNodeAllocator& memoryBlockNodeAllocator) noexcept:
        _slotSize(slotSize),
        _slotsPerChunkCount(slotsPerChunkCount),
        _pageAllocator(pageAllocator),
        _memoryBlockNodeAllocator(memoryBlockNodeAllocator)
        {
            assert::debug(slotSize > 0, "SlotSize must be greater than zero");
            assert::debug(slotSize > sizeof(void*), "SlotSize must be at least as big as size of a pointer");
            assert::debug(slotsPerChunkCount > 0, "SlotsPerChunkCount must be greater than zero");
            const std::size_t slotsPerPageCount = _pageAllocator.getPageSize() / _slotSize;
            assert::debug(slotsPerChunkCount % slotsPerPageCount == 0, "Slot per chunk must be a multiple of slots per page");
            assert::debug(_pageAllocator.getPageSize() >= slotSize, "SlotSize must fit into a memory page");
            const std::size_t mask = slotSize - 1;
            assert::debug((slotSize & mask) == 0, "SlotSize must be a power of 2");

            allocateChunk(); //Allocate the initial memory chunk
        }

        ~FreeListAllocator()
        {
            MemoryBlockNode* node = _memoryBlockHead;

            while(node != nullptr)
            {
                if(node->memoryBlock.ptr != nullptr)
                {
                    _pageAllocator.deallocate(node->memoryBlock);
                }

                _memoryBlockNodeAllocator.deallocate(node);

                node = node->previousNode;
            }
        }

        void* allocate() noexcept
        {
            void* headPointer = _freeListHead.load(std::memory_order::relaxed);
            if(headPointer == nullptr)
            {
                allocateChunk(); //Allocate an extra chunk of memory

                headPointer = _freeListHead.load(std::memory_order::relaxed);
                if(headPointer == nullptr)
                {
                    assert::release(false, "Out of memory");
                    return nullptr;
                }
            }

            //Read pointer to the next free slot, replace head pointer with the next slot pointer and return the current head pointer to the consumer
            void* nextSlotPointer = *static_cast<void* const*>(headPointer);
            while(!_freeListHead.compare_exchange_weak(headPointer, nextSlotPointer, std::memory_order::acquire, std::memory_order::relaxed))
            {
                if(headPointer == nullptr)
                {
                    allocateChunk();

                    headPointer = _freeListHead.load(std::memory_order::relaxed);
                    if(headPointer == nullptr)
                    {
                        assert::release(false, "Out of memory");
                        return nullptr;
                    }
                }

                nextSlotPointer = *static_cast<void* const*>(headPointer);
            }

            return headPointer;
        }

        void deallocate(void* ptr) noexcept
        {
            assert::inhouse(isInRange(ptr), "Pointer does not belong to the managed memory of the allocator");

            //Write current head pointer value to the deallocated slot and write address of the deallocated slot to the head pointer
            void* headPointer = _freeListHead.load(std::memory_order::relaxed);
            *static_cast<void**>(ptr) = headPointer;
            while(!_freeListHead.compare_exchange_weak(headPointer, ptr, std::memory_order::release, std::memory_order::relaxed))
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

    private:
        void allocateChunk()
        {
            //Allocate a new MemoryBlockNode as well as a MemoryBlock
            MemoryBlockNode* pendingBlock = _pendingBlock.load(std::memory_order::acquire);
            if(pendingBlock == nullptr)
            {
                auto newBlock = static_cast<MemoryBlockNode*>(_memoryBlockNodeAllocator.allocate(sizeof(MemoryBlockNode)));

                if(newBlock == nullptr)
                {
                    assert::release(false, "Cannot allocate memory block node");
                    return;
                }

                const std::size_t slotsPerPageCount = _pageAllocator.getPageSize() / _slotSize;
                const std::size_t pagesCount = _slotsPerChunkCount / slotsPerPageCount;

                const ErrorCode errorCode = _pageAllocator.allocate(pagesCount, newBlock->memoryBlock);
                if(errorCode != ErrorCode::Success)
                {
                    assert::release(false, "Cannot allocate memory block");
                    return;
                }

                if(_pendingBlock.compare_exchange_strong(pendingBlock, newBlock, std::memory_order::release, std::memory_order::acquire))
                {
                    pendingBlock = newBlock;
                }
                else
                {
                    _pageAllocator.deallocate(newBlock->memoryBlock);
                    _memoryBlockNodeAllocator.deallocate(newBlock);
                }
            }

            //Initialize linked list in the memory block
            MemoryBlock& memoryBlock = pendingBlock->memoryBlock;
            std::size_t currentSlot = _currentSlotInitialization.load(std::memory_order::relaxed);
            while(currentSlot < _slotsPerChunkCount)
            {
                std::size_t nextSlot = currentSlot + 1;
                if(!_currentSlotInitialization.compare_exchange_strong(currentSlot, nextSlot, std::memory_order::relaxed, std::memory_order::relaxed))
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

                currentSlot = _currentSlotInitialization.load(std::memory_order::relaxed);
            }

            //Update bookkeeping references
            _currentSlotInitialization.compare_exchange_strong(currentSlot, 0, std::memory_order::relaxed, std::memory_order::relaxed);

            if(_pendingBlock.compare_exchange_strong(pendingBlock, nullptr, std::memory_order::release, std::memory_order::acquire))
            {
                MemoryBlockNode* headNode = _memoryBlockHead.load(std::memory_order::relaxed);
                pendingBlock->previousNode = headNode;
                while(!_memoryBlockHead.compare_exchange_weak(headNode, pendingBlock, std::memory_order::release, std::memory_order::relaxed))
                {
                    pendingBlock->previousNode = headNode;
                }

                void* freeListHead = _freeListHead.load(std::memory_order::relaxed);
                const std::uintptr_t slotAddress = reinterpret_cast<std::uintptr_t>(memoryBlock.ptr) + _slotSize * (_slotsPerChunkCount - 1);
                *reinterpret_cast<void**>(slotAddress) = freeListHead;
                while(!_freeListHead.compare_exchange_weak(freeListHead, memoryBlock.ptr, std::memory_order::release, std::memory_order::relaxed))
                {
                    *reinterpret_cast<void**>(slotAddress) = freeListHead;
                }
            }
        }
    };
}
