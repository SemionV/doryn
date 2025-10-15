#pragma once
#include <atomic>
#include <cstddef>
#include "pageAllocator.h"

namespace dory::memory
{
    struct MemoryBlockNode
    {
        MemoryBlock memoryBlock;
        MemoryBlockNode* previousNode = nullptr;
    };

    template<typename T, typename TPageAllocator, template<typename> class TMemoryBlockNodeAllocator>
    class FreeListAllocator
    {
    private:
        const std::size_t _slotSize;
        const std::size_t _slotsPerChunkCount;
        TPageAllocator& _pageAllocator;
        TMemoryBlockNodeAllocator<MemoryBlockNode>& _memoryBlockNodeAllocator;
        alignas(64) std::atomic<T*> _freeListHead;
        alignas(64) std::atomic<MemoryBlockNode*> _memoryBlockHead;
        alignas(64) std::atomic<MemoryBlockNode*> _pendingBlock;
        alignas(64) std::atomic<std::size_t> _currentSlotInitialization;

    public:
        FreeListAllocator(const std::size_t slotSize, const std::size_t slotsPerChunkCount,
            TPageAllocator& pageAllocator,
            TMemoryBlockNodeAllocator<MemoryBlockNode>& memoryBlockNodeAllocator) noexcept:
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

            allocateChunk();
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

        T* allocate() noexcept
        {
            T* headPointer = _freeListHead.load(std::memory_order::relaxed);
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

            T* nextSlotPointer = *static_cast<T* const*>(headPointer);
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

                nextSlotPointer = *static_cast<T* const*>(headPointer);
            }

            return headPointer;
        }

        void deallocate(T* ptr) noexcept
        {
            MemoryBlockNode* node = _memoryBlockHead;
            bool isInRange = false;

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

            assert::inhouse(isInRange, "Pointer does not belong to the managed memory of the allocator");

            while(true)
            {
                void* headPointer = _freeListHead.load(std::memory_order::relaxed);

                *static_cast<T**>(ptr) = headPointer;

                if(_freeListHead.compare_exchange_weak(headPointer, ptr, std::memory_order::release, std::memory_order::relaxed)) [[likely]]
                {
                    break;
                }
            }
        }

    private:
        void allocateChunk()
        {
            //Allocate a new MemoryBlockNode as well as a MemoryBlock
            MemoryBlockNode* memoryBlockInitialization = _pendingBlock.load(std::memory_order::acquire);
            if(memoryBlockInitialization == nullptr)
            {
                MemoryBlockNode* memoryBlockNode = _memoryBlockNodeAllocator.allocate();

                if(memoryBlockNode == nullptr)
                {
                    assert::release(false, "Cannot allocate memory block node");
                    return;
                }

                const std::size_t slotsPerPageCount = _pageAllocator.getPageSize() / _slotSize;
                const std::size_t pagesCount = _slotsPerChunkCount / slotsPerPageCount;

                const ErrorCode errorCode = _pageAllocator.allocate(pagesCount, memoryBlockNode->memoryBlock);
                if(errorCode != ErrorCode::Success)
                {
                    assert::release(false, "Cannot allocate memory block");
                    return;
                }

                if(_pendingBlock.compare_exchange_strong(memoryBlockInitialization, memoryBlockNode, std::memory_order::release, std::memory_order::acquire))
                {
                    memoryBlockInitialization = memoryBlockNode;
                }
                else
                {
                    _pageAllocator.deallocate(memoryBlockNode->memoryBlock);
                    _memoryBlockNodeAllocator.deallocate(memoryBlockNode);
                }
            }

            //Initialize linked list in the memory block
            MemoryBlock& memoryBlock = memoryBlockInitialization->memoryBlock;
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

            if(_pendingBlock.compare_exchange_strong(memoryBlockInitialization, nullptr, std::memory_order::release, std::memory_order::acquire))
            {
                MemoryBlockNode* headNode = _memoryBlockHead.load(std::memory_order::relaxed);
                memoryBlockInitialization->previousNode = headNode;
                while(!_memoryBlockHead.compare_exchange_weak(headNode, memoryBlockInitialization, std::memory_order::release, std::memory_order::relaxed))
                {
                    memoryBlockInitialization->previousNode = headNode;
                }

                T* freeListHead = _freeListHead.load(std::memory_order::relaxed);
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
