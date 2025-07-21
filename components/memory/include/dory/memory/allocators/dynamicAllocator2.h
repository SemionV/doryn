#pragma once

#include <thread>
#include <dory/macros/assert.h>
#include <dory/macros/constants.h>
#include <dory/memory/resources/memoryBlock.h>
#include <dory/memory/resources/resourceHandle.h>
#include <dory/concurrency/spinLock.h>
#include "poolAllocator.h"

namespace dory::memory
{
    struct alignas(constants::CACHELINE_SIZE) DynamicBlock //alignas: avoid fake sharing and cache ping-pong between CPU cores
    {
        using MutexType = concurrency::SpinLockMutex;

        void* ptr {};
        std::size_t size {};
        DynamicBlock* nextBlock {};
        std::size_t state {};
        std::uint8_t alignPower {};
        std::atomic<std::uint8_t> refCount {}; //count of threads entered the chain after the block, needed for garbage collector
        MutexType mutex;
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
                std::pair<DynamicBlock*, std::size_t> result = tryAcquireBlock(size, getAlignPower(align), block);

                block = block->nextBlock;
            }

            return ResourceHandle<T>{ {} };
        }

    private:
        int static getAlignPower(std::uint16_t align) noexcept
        {
            assert::debug(align, "Alignment cannot be 0");
            assert::debug((align & align - 1) == 0, "Alignment must be a power of 2");

            int position = 0;
            while (align >>= 1)
            {
                ++position;
            }
            return position;
        }

        static std::size_t getAlignedOffset(const std::uint8_t alignPower, const DynamicBlock& block) noexcept
        {
            const auto address = reinterpret_cast<std::uintptr_t>(block.ptr);
            const std::uintptr_t alignedAddress = alignAddress(address, 1 << alignPower);
            return alignedAddress - address;
        }

        static std::pair<DynamicBlock*, std::size_t> tryAcquireBlock(const std::size_t size, const std::uint8_t alignPower, DynamicBlock* block) noexcept
        {
            std::pair<DynamicBlock*, std::size_t> result { block, 0 };

            std::lock_guard lock {block->mutex};

            if(DynamicBlockState::isFree(block->state))
            {
                const std::size_t alignedOffset = getAlignedOffset(alignPower, *block);
                if(alignedOffset < block->size)
                {
                    const std::size_t sizeWithAlignment = size + alignedOffset;
                    block->state = sizeWithAlignment;
                    block->alignPower = alignPower;

                    if(sizeWithAlignment > block->size)
                    {
                        result.first = block->nextBlock;
                        result.second = block->size - alignedOffset;
                    }
                }
            }
            else if(DynamicBlockState::isLocked(block->state))
            {
                result.first = fastForwardLockedChain(block);
            }
            else //block is allocated
            {
                result.first = block->nextBlock;
            }

            return result;
        }

        static DynamicBlock* fastForwardLockedChain(DynamicBlock* lockedBlock) noexcept
        {
            DynamicBlock* freeBlock = nullptr;

            while(lockedBlock)
            {
                const std::size_t lockedSize = lockedBlock->state;
                const std::size_t lockedOffset = getAlignedOffset(lockedBlock->alignPower, *lockedBlock);
                std::size_t accumulatedSize = lockedBlock->size - lockedOffset;

                DynamicBlock* currentBlock = lockedBlock->nextBlock;
                DynamicBlock* beginBlock = currentBlock;
                if(beginBlock)
                {

                }

                lockedBlock->mutex.unlock();

                while(accumulatedSize < lockedSize)
                {
                    if(currentBlock)
                    {
                        std::lock_guard lock {currentBlock->mutex};

                        if(DynamicBlockState::isFree(currentBlock->state))
                        {
                            accumulatedSize += currentBlock->size;
                            currentBlock = currentBlock->nextBlock;
                            if(accumulatedSize >= lockedSize)
                            {
                                freeBlock = currentBlock;
                                lockedBlock = nullptr;
                                break;
                            }
                        }
                        else if(DynamicBlockState::isLocked(currentBlock->state))
                        {
                            //restart search for a free block from the current locked block
                            lockedBlock = currentBlock;
                            break;
                        }
                        else
                        {
                            freeBlock = currentBlock->nextBlock;
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }

                beginBlock->refCount.fetch_sub(1);
            }

            return freeBlock;
        }
    };
}