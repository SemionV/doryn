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
        std::atomic<std::size_t> lock {};
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

            //aligned size(usually should be pre-calculated by the compiler)
            const std::size_t align = alignof(T);
            const std::size_t size = sizeof(T);
            const std::size_t sizeWithAlignment = size + align - 1;

            DynamicBlock* block = _headBlock;
            while(block)
            {
                std::size_t lockValue = DynamicBlockState::free;
                if(block->lock.compare_exchange_strong(lockValue, sizeWithAlignment))
                {
                    const std::uintptr_t alignedOffset = getAlignedOffset(align, block);
                    std::size_t acquiredSize = block->size - alignedOffset;
                    DynamicBlock* currentBlock = block->nextBlock;
                    //try to acquire blocks for requested size
                    while(acquiredSize < size)
                    {
                        if(currentBlock)
                        {
                            lockValue = DynamicBlockState::free;
                            if(currentBlock->lock.compare_exchange_strong(lockValue, size - acquiredSize))
                            {
                                acquiredSize += currentBlock->size;
                            }
                            else
                            {
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
                        //TODO: release acquired blocks
                    }
                }
                else if(DynamicBlockState::isLocked(lockValue))
                {

                }

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
    };
}