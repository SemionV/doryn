#pragma once

#include <dory/macros/assert.h>
#include <dory/macros/constants.h>
#include <dory/memory/resources/memoryBlock.h>
#include <dory/memory/resources/resourceHandle.h>
#include "poolAllocator.h"

namespace dory::memory
{
    struct alignas(constants::cacheLineSize) DynamicBlock //alignas: avoid fake sharing and cache ping-pong between CPU cores
    {
        std::size_t offset {};
        std::size_t size {};
        DynamicBlock* nextBlock {};
        std::atomic<std::size_t> state {};
    };

    struct DynamicBlockState
    {
        constexpr static std::size_t free = 0;
        constexpr static std::size_t allocated = std::numeric_limits<std::size_t>::max();

        static bool isFree(const std::size_t state)
        {
            return state == free;
        }

        static bool isAllocated(const std::size_t state)
        {
            return state == allocated;
        }

        static bool isLocked(const std::size_t state)
        {
            return state != allocated && state != free;
        }
    };

    struct AddressHandle
    {
        DynamicBlock* block {};
        std::atomic<std::size_t> referenceCount {};
    };

    template<typename TPolicies>
    class DynamicAllocator2
    {
        MemoryBlock _memory;
        PoolAllocator<DynamicBlock>& _blocksAllocator;
        PoolAllocator<AddressHandle>& _handlesAllocator;
        DynamicBlock* _headBlock {};

    public:
        explicit DynamicAllocator2(const MemoryBlock& memoryBlock,
            PoolAllocator<DynamicBlock>& blocksAllocator,
            PoolAllocator<AddressHandle>& handlesAllocator) noexcept
            : _memory(memoryBlock),
            _blocksAllocator(blocksAllocator),
            _handlesAllocator(handlesAllocator)
        {}

        template<typename T>
        ResourceHandle<T> allocate() noexcept
        {
            assert::debug(_headBlock, "Head block descriptor is not allocated");

            const std::size_t align = alignof(T);
            const std::size_t size = sizeof(T);

            DynamicBlock* block = _headBlock;
            while(block)
            {
                DynamicBlock* resultBlock = tryAcquireBlock(size, align, block);

                block = block->nextBlock;
            }

            return ResourceHandle<T>{ {} };
        }

    private:
        std::size_t getAlignedOffset(const std::size_t align, const DynamicBlock& block)
        {
            const std::uintptr_t address = reinterpret_cast<std::uintptr_t>(_memory.ptr) + block.offset;
            const std::uintptr_t alignedAddress = alignAddress(address, align);
            return alignedAddress - address - block.offset;
        }

        DynamicBlock* tryAcquireBlock(const std::size_t size, const std::size_t align, DynamicBlock* block)
        {
            DynamicBlock* result = block;
            std::size_t sizeWithAlignment = size + align - 1;
            std::size_t stateValue = DynamicBlockState::free;

            if(block->state.compare_exchange_strong(stateValue, sizeWithAlignment))
            {
                const std::uintptr_t alignedOffset = getAlignedOffset(align, block);
                if(alignedOffset < block->size)
                {
                    std::size_t acquiredSize = block->size - alignedOffset;
                    DynamicBlock* currentBlock = block->nextBlock;

                    while(acquiredSize < size)
                    {
                        if(currentBlock)
                        {
                            stateValue = DynamicBlockState::free;
                            if(currentBlock->state.compare_exchange_strong(stateValue, size - acquiredSize))
                            {
                                acquiredSize += currentBlock->size;
                            }
                            else if(DynamicBlockState::isLocked(stateValue))
                            {
                                //TODO: if currentBlock is locked for potential allocation,
                                //TODO: we can wait until the block changes the state to free or
                                //TODO: allocated in order to not waste an opportunity if the block will be released by the other thread
                            }
                            else
                            {
                                //the block is allocated
                                break;
                            }

                            currentBlock = currentBlock->nextBlock;
                        }
                        else
                        {
                            break;
                        }
                    }

                    //blocks are successfully acquired
                    if(acquiredSize >= size)
                    {
                        //TODO: create a free block for the alignment gap in the beginning of the first segment
                        //TODO: merge all allocated blocks
                        //TODO: create a free block for the rest at the last segment
                    }
                    else
                    {
                        //release acquired blocks

                        result = currentBlock;
                        bool released = block->state.compare_exchange_strong(sizeWithAlignment, DynamicBlockState::free);
                        assert::debug(released, "Block is not released");

                        std::size_t releasedSize = block->size - alignedOffset;
                        currentBlock = block->nextBlock;
                        std::size_t expectedState = size - releasedSize;
                        while(releasedSize < acquiredSize)
                        {
                            released = currentBlock->state.compare_exchange_strong(expectedState, DynamicBlockState::free);
                            assert::debug(released, "Block is not released");
                            releasedSize += currentBlock->size;
                            expectedState = size - releasedSize;

                            currentBlock = currentBlock->nextBlock;
                        }
                    }
                }
                else
                {
                    const bool released = block->state.compare_exchange_strong(sizeWithAlignment, DynamicBlockState::free);
                    assert::debug(released, "Block is not released");
                }
            }
            else if(DynamicBlockState::isLocked(stateValue))
            {

            }

            return result;
        }
    };
}