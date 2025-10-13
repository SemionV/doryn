#pragma once
#include <atomic>
#include <cstddef>
#include "pageAllocator.h"

namespace dory::memory
{
    template<typename TPageAllocator>
    class FreeListAllocator
    {
    private:
        const std::size_t _slotSize;
        const std::size_t _slotsPerChunkCount;
        TPageAllocator& _pageAllocator;
        std::atomic<void*> _freeListHead = nullptr;
        MemoryBlock _memoryBlock;

    public:
        FreeListAllocator(const std::size_t slotSize, const std::size_t slotsPerChunkCount, TPageAllocator& pageAllocator) noexcept:
        _slotSize(slotSize),
        _slotsPerChunkCount(slotsPerChunkCount),
        _pageAllocator(pageAllocator)
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
            if(_memoryBlock.ptr != nullptr)
            {
                _pageAllocator.deallocate(_memoryBlock);
            }
        }

        void* allocateSlot() noexcept
        {
            while(true)
            {
                void* headPointer = _freeListHead.load(std::memory_order::relaxed);

                if(headPointer != nullptr)
                {
                    void* nextSlotPointer = *static_cast<void* const*>(headPointer);
                    if(_freeListHead.compare_exchange_weak(headPointer, nextSlotPointer, std::memory_order::acquire, std::memory_order::relaxed))
                    {
                        return headPointer;
                    }

                    continue;
                }

                assert::release(false, "Out of memory");
                return nullptr;
            }
        }

        void deallocateSlot(void* ptr) noexcept
        {
            const auto chunkStartAddress = reinterpret_cast<uintptr_t>(_memoryBlock.ptr);
            const std::uintptr_t chunkEndAddress = chunkStartAddress + _memoryBlock.pageSize * _memoryBlock.pagesCount;
            const auto address = reinterpret_cast<uintptr_t>(ptr);
            assert::inhouse(address >= chunkStartAddress && address <= chunkEndAddress - sizeof(void*), "Pointer does not belong to the managed memory block");

            while(true)
            {
                void* headPointer = _freeListHead.load(std::memory_order::relaxed);

                *static_cast<void**>(ptr) = headPointer;

                if(_freeListHead.compare_exchange_weak(headPointer, ptr, std::memory_order::release, std::memory_order::relaxed)) [[likely]]
                {
                    break;
                }
            }
        }

    private:
        void allocateChunk()
        {
            const std::size_t slotsPerPageCount = _pageAllocator.getPageSize() / _slotSize;
            const std::size_t pagesCount = _slotsPerChunkCount / slotsPerPageCount;

            const ErrorCode errorCode = _pageAllocator.allocate(pagesCount, _memoryBlock);
            if(errorCode != ErrorCode::Success)
            {
                assert::release(false, "Cannot allocate memory block");
            }

            for(std::size_t i = 0; i < _slotsPerChunkCount; ++i)
            {
                const std::uintptr_t slotAddress = reinterpret_cast<std::uintptr_t>(_memoryBlock.ptr) + _slotSize * i;

                if(i != _slotsPerChunkCount - 1)
                {
                    const std::uintptr_t nextSlotAddress = reinterpret_cast<std::uintptr_t>(_memoryBlock.ptr) + _slotSize * (i + 1);
                    *reinterpret_cast<std::uintptr_t*>(slotAddress) = nextSlotAddress;
                }
                else
                {
                    *reinterpret_cast<void**>(slotAddress) = nullptr;
                }
            }

            _freeListHead = _memoryBlock.ptr;
        }
    };
}
