#pragma once

#include <atomic>
#include <dory/memory/allocation.h>
#include <dory/memory/resources/memoryBlock.h>
#include <dory/memory/resources/resourceHandle.h>
#include "poolAllocator.h"

namespace dory::memory
{
    enum class DynamicBlockState
    {
        free,
        locked,
        allocated
    };

    struct DynamicBlock
    {
        std::size_t offset {};
        std::size_t size {};
        DynamicBlock* nextBlock {};
        std::atomic<DynamicBlockState> state;

        bool allocated {}; //TODO: redundant, remove
    };

    struct AddressHandle
    {
        DynamicBlock* descriptor {};
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
        PoolAllocator<DynamicBlock>& _blocksAllocator;
        PoolAllocator<AddressHandle>& _handlesAllocator;
        DynamicBlock* _headBlock {};

        std::size_t allocate(const std::size_t alignedAddress, const std::size_t size, DynamicBlock* freeBlock, DynamicBlock* prevBlock)
        {
            DynamicBlock* allocatedBlock = _blocksAllocator.allocate();
            allocatedBlock->size = size;
            allocatedBlock->offset = alignedAddress;
            allocatedBlock->allocated = true;

            DynamicBlock* leftBlock = allocatedBlock;

            //If there is a gap because of address alignment, create an empty block for the gap
            if(const std::size_t gapSize = alignedAddress - freeBlock->offset)
            {
                DynamicBlock* gapBlock = leftBlock = _blocksAllocator.allocate();
                gapBlock->offset = freeBlock->offset;
                gapBlock->size = gapSize;
                gapBlock->nextBlock = allocatedBlock;
            }

            const std::size_t effectiveSize = alignedAddress + size - freeBlock->offset;

            if(const std::size_t leftoverSize = freeBlock->size - effectiveSize)
            {
                DynamicBlock* leftoverBlock = _blocksAllocator.allocate();
                leftoverBlock->offset = freeBlock->offset + effectiveSize;
                leftoverBlock->size = leftoverSize;
                leftoverBlock->nextBlock = freeBlock->nextBlock;
                allocatedBlock->nextBlock = leftoverBlock;
            }
            else
            {
                allocatedBlock->nextBlock = freeBlock->nextBlock;
            }

            {
                //TODO: critical section, check if other thread have not changed prevBlock->nextBlock value
                //TODO: deallocate allocatedBlock, gapBlock and leftoverBlock if allocation has to be restarted

                //TODO: use atomic CAS operation
                prevBlock->nextBlock = leftBlock;

                _blocksAllocator.deallocate(freeBlock);
            }

            return 0;
        }

        std::size_t allocateLikeABoss(const std::size_t alignedAddress, const std::size_t size, DynamicBlock* freeBlock)
        {
            return 0;
        }

    public:

        explicit DynamicAllocator(const MemoryBlock& memoryBlock,
            PoolAllocator<DynamicBlock>& descriptorsAllocator,
            PoolAllocator<AddressHandle>& handlesAllocator) noexcept;

        template<typename T>
        ResourceHandle<T> allocate() noexcept
        {
            assert::debug(_headBlock, "Head block descriptor is not allocated");

            //aligned size(usually should be pre-calculated by the compiler)
            const std::size_t align = alignof(T);
            const std::size_t size = alignAddress(sizeof(T), align);

            DynamicBlock* block = _headBlock;
            while(block)
            {
                if(block->state.load(std::memory_order_acquire) == DynamicBlockState::free)

                if(!block->allocated) //TODO: use atomic read operation of the flag
                {
                    const std::size_t alignedAddress = alignAddress(block->offset, align);
                    if(alignedAddress + size < block->offset + block->size)
                    {
                        return ResourceHandle<T>{ allocateLikeABoss(alignedAddress, size, block) };
                    }
                }

                //TODO: use atomic read operation
                block = block->nextBlock;
            }

            return ResourceHandle<T>{ {} };
        }

        template<typename T>
        void deallocate(ResourceHandle<T>& handle) noexcept
        {
        }
    };
}