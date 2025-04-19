#pragma once

#include <atomic>
#include <dory/memory/allocation.h>
#include <dory/memory/resources/memoryBlock.h>
#include <dory/memory/resources/resourceHandle.h>
#include "poolAllocator.h"

namespace dory::memory
{
    enum class BlockDescriptorState
    {
        free,
        locked,
        allocated
    };

    struct BlockDescriptor
    {
        std::size_t offset {};
        std::size_t size {};
        BlockDescriptor* prevDescriptor {};
        BlockDescriptor* nextDescriptor {};

        bool allocated {};
    };

    struct AddressHandle
    {
        BlockDescriptor* descriptor {};
        std::atomic<bool> locked {};
    };

    //TODO: implement lock-free multithreading support
    //TODO: implement aligned allocation(cache line, etc)
    //TODO: implement best-fit strategy for selecting a free block
    //TODO: implement coalescing
    //TODO: implement defragmentation
    class DynamicAllocator
    {
    private:
        MemoryBlock _memory;
        PoolAllocator<BlockDescriptor>& _descriptorsAllocator;
        PoolAllocator<AddressHandle>& _handlesAllocator;
        BlockDescriptor* _headDescriptor {};

        std::size_t allocate(const std::size_t alignedAddress, const std::size_t size, BlockDescriptor* freeBlock, BlockDescriptor* prevBlock)
        {
            BlockDescriptor* allocatedBlock = _descriptorsAllocator.allocate();
            allocatedBlock->size = size;
            allocatedBlock->offset = alignedAddress;
            allocatedBlock->allocated = true;

            BlockDescriptor* leftBlock = allocatedBlock;

            //If there is a gap because of address alignment, create an empty block for the gap
            if(const std::size_t gapSize = alignedAddress - freeBlock->offset)
            {
                BlockDescriptor* gapBlock = leftBlock = _descriptorsAllocator.allocate();
                gapBlock->offset = freeBlock->offset;
                gapBlock->size = gapSize;
                gapBlock->prevDescriptor = freeBlock->prevDescriptor;
                gapBlock->nextDescriptor = allocatedBlock;
                allocatedBlock->prevDescriptor = gapBlock;
            }
            else
            {
                allocatedBlock->prevDescriptor = freeBlock->prevDescriptor;
            }

            const std::size_t effectiveSize = alignedAddress + size - freeBlock->offset;

            if(const std::size_t leftoverSize = freeBlock->size - effectiveSize)
            {
                BlockDescriptor* leftoverBlock = _descriptorsAllocator.allocate();
                leftoverBlock->offset = freeBlock->offset + effectiveSize;
                leftoverBlock->size = leftoverSize;
                leftoverBlock->prevDescriptor = allocatedBlock;
                leftoverBlock->nextDescriptor = freeBlock->nextDescriptor;
                allocatedBlock->nextDescriptor = leftoverBlock;
            }
            else
            {
                allocatedBlock->nextDescriptor = freeBlock->nextDescriptor;
            }

            {
                //TODO: critical section, check if other thread have not changed prevBlock->nextBlock value
                //TODO: deallocate allocatedBlock, gapBlock and leftoverBlock if allocation has to be restarted

                //TODO: use atomic CAS operation
                prevBlock->nextDescriptor = leftBlock;

                _descriptorsAllocator.deallocate(freeBlock);
            }
        }

    public:
        template<typename T> friend class ResourceHandle<T>;

        explicit DynamicAllocator(const MemoryBlock& memoryBlock,
            PoolAllocator<BlockDescriptor>& descriptorsAllocator,
            PoolAllocator<AddressHandle>& handlesAllocator) noexcept;

        template<typename T>
        ResourceHandle<T> allocate() noexcept
        {
            assert::debug(_headDescriptor, "Head block descriptor is not allocated");

            //aligned size(usually should be pre-calculated by the compiler)
            const std::size_t align = alignof(T);
            const std::size_t size = alignAddress(sizeof(T), align);

            BlockDescriptor* prevBlock = _headDescriptor;
            BlockDescriptor* descriptor = _headDescriptor->nextDescriptor;
            while(descriptor)
            {
                if(!descriptor->allocated)
                {
                    const std::size_t alignedAddress = alignAddress(descriptor->offset, align);
                    if(alignedAddress + size < descriptor->offset + descriptor->size)
                    {
                        return ResourceHandle<T>{ allocate(alignedAddress, size, descriptor, prevBlock) };
                    }
                }

                prevBlock = descriptor;
                //TODO: use atomic read operation
                descriptor = descriptor->nextDescriptor;
            }

            return ResourceHandle<T>{ {} };
        }

        template<typename T>
        void deallocate(ResourceHandle<T>& handle) noexcept
        {
        }
    };
}