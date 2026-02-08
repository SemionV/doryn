#pragma once
#include <atomic>
#include <cstddef>
#include "pageAllocator.h"
#include <spdlog/fmt/fmt.h>

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
        const std::size_t _chunkSize;
        const std::size_t _pageSize;
        const std::size_t _pageCount;
        TPageAllocator& _pageAllocator; //Allocator of memory chunks
        TMemoryBlockNodeAllocator& _memoryBlockNodeAllocator; //Allocator of MemoryBlock descriptors, wrapped in a Node structure, to make a linked list of all allocated chunks
        alignas(64) std::atomic<void*> _freeListHead; //Pointer to the first node in the free list
        alignas(64) std::atomic<MemoryBlockNode*> _memoryBlockHead; //Pointer to the last node of allocated memory chunks
        alignas(64) std::atomic<MemoryBlockNode*> _pendingBlock; //A newly allocated memory chunk, which is in progress of initialization, each thread, which is seeing it can help to initialize it
        alignas(64) std::atomic<std::size_t> _currentSlotInitialization; //Index of a slot in _pendingBlock, which currently on initialization, each thread can peek one

        struct MemOrder
        {
            static constexpr auto relaxed = std::memory_order::seq_cst;
            static constexpr auto acquire = std::memory_order::seq_cst;
            static constexpr auto release = std::memory_order::seq_cst;
            static constexpr auto seq_cst = std::memory_order::seq_cst;
        };

    public:
        FreeListAllocator(const std::size_t slotSize, const std::size_t slotsPerChunkCount,
            TPageAllocator& pageAllocator,
            TMemoryBlockNodeAllocator& memoryBlockNodeAllocator) noexcept:
        _slotSize(slotSize),
        _slotsPerChunkCount(slotsPerChunkCount),
        _chunkSize(slotSize * slotsPerChunkCount),
        _pageAllocator(pageAllocator),
        _pageSize(pageAllocator.getPageSize()),
        _pageCount(_chunkSize / _pageSize),
        _memoryBlockNodeAllocator(memoryBlockNodeAllocator)
        {
            assert::debug(slotSize > 0, "SlotSize must be greater than zero");
            assert::debug(slotSize >= sizeof(void*), fmt::format("SlotSize must be at least as big as size of a pointer. SlotSize: {}", slotSize).c_str());
            assert::debug(slotsPerChunkCount > 0, "SlotsPerChunkCount must be greater than zero");
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

                MemoryBlockNode* prevNode = node->previousNode;
                _memoryBlockNodeAllocator.deallocate(node);
                node = prevNode;
            }
        }

        void* allocate() noexcept
        {
            void* headPointer = _freeListHead.load(MemOrder::relaxed);
            if(headPointer == nullptr)
            {
                allocateChunk(); //Allocate an extra chunk of memory

                headPointer = _freeListHead.load(MemOrder::relaxed);
                if(headPointer == nullptr)
                {
                    assert::release(false, "Out of memory");
                    return nullptr;
                }
            }

            //Read pointer to the next free slot, replace head pointer with the next slot pointer and return the current head pointer to the consumer
            void* nextSlotPointer = *static_cast<void* const*>(headPointer);
            while(!_freeListHead.compare_exchange_weak(headPointer, nextSlotPointer, MemOrder::acquire, MemOrder::relaxed))
            {
                if(headPointer == nullptr)
                {
                    allocateChunk();

                    headPointer = _freeListHead.load(MemOrder::relaxed);
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
        void allocateChunk()
        {
            //Allocate a new MemoryBlockNode as well as a MemoryBlock
            MemoryBlockNode* pendingBlock = _pendingBlock.load(MemOrder::acquire);
            if(pendingBlock == nullptr)
            {
                auto newBlock = static_cast<MemoryBlockNode*>(_memoryBlockNodeAllocator.allocate(sizeof(MemoryBlockNode)));

                if(newBlock == nullptr)
                {
                    assert::release(false, "Cannot allocate memory block node");
                    return;
                }

                std::size_t pagesCount = _pageCount == 0 ? 1 : _pageSize;

                const ErrorCode errorCode = _pageAllocator.allocate(pagesCount, newBlock->memoryBlock);
                if(errorCode != ErrorCode::Success)
                {
                    assert::release(false, "Cannot allocate memory block");
                    return;
                }

                if(_pendingBlock.compare_exchange_strong(pendingBlock, newBlock, MemOrder::release, MemOrder::acquire))
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
            std::size_t currentSlot = _currentSlotInitialization.load(MemOrder::relaxed);
            while(currentSlot < _slotsPerChunkCount)
            {
                std::size_t nextSlot = currentSlot + 1;
                if(!_currentSlotInitialization.compare_exchange_strong(currentSlot, nextSlot, MemOrder::relaxed, MemOrder::relaxed))
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

                currentSlot = _currentSlotInitialization.load(MemOrder::relaxed);
            }

            //Update bookkeeping references
            _currentSlotInitialization.compare_exchange_strong(currentSlot, 0, MemOrder::relaxed, MemOrder::relaxed);

            if(_pendingBlock.compare_exchange_strong(pendingBlock, nullptr, MemOrder::release, MemOrder::acquire))
            {
                MemoryBlockNode* headNode = _memoryBlockHead.load(MemOrder::relaxed);
                pendingBlock->previousNode = headNode;
                while(!_memoryBlockHead.compare_exchange_weak(headNode, pendingBlock, MemOrder::release, MemOrder::relaxed))
                {
                    pendingBlock->previousNode = headNode;
                }

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
                assert::debug(false, "Lost pending block\n");
            }
        }
    };
}
