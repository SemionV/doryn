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
    struct alignas(constants::cacheLineSize) DynamicBlock //alignas: avoid fake sharing and cache ping-pong between CPU cores
    {
        void* ptr {};
        std::size_t size {};
        DynamicBlock* nextBlock {};
        std::size_t state {};
        std::uint8_t alignPower {};
        std::uint8_t refCount {}; //count of threads entered the chain after the block, needed for garbage collector
        std::atomic_flag lockFlag {};
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
        int static getAlignPower(std::uint16_t align)
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

        static std::size_t getAlignedOffset(const std::uint8_t alignPower, const DynamicBlock& block)
        {
            const auto address = reinterpret_cast<std::uintptr_t>(block.ptr);
            const std::uintptr_t alignedAddress = alignAddress(address, 1 << alignPower);
            return alignedAddress - address;
        }

        static void acquireLock(DynamicBlock* block)
        {
            while(block->lockFlag.test_and_set(std::memory_order_acquire))
            {
                block->lockFlag.wait(true, std::memory_order_relaxed);
            }
        }

        static void releaseLock(DynamicBlock* block)
        {
            block->lockFlag.clear(std::memory_order_release);
            block->lockFlag.notify_one();
        }

        static std::pair<DynamicBlock*, std::size_t> tryAcquireBlock(const std::size_t size, const std::uint8_t alignPower, DynamicBlock* block)
        {
            std::pair<DynamicBlock*, std::size_t> result { block, 0 };

            acquireLock(block); //TODO: use RAII

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
                if(DynamicBlock* freeBlock = fastForwardLockedChain(block))
                {
                    //TODO: handle the case
                    result.first = freeBlock;
                }
                else
                {
                    //TODO: handle nullptr case
                }

                //TODO: traverse the chain to find next block beyond the lock

                std::size_t lockedSize = block->state;
                std::size_t lockedOffset = getAlignedOffset(block->alignPower, *block);
                std::size_t accumulatedSize = block->size - lockedOffset;

                if(accumulatedSize >= lockedSize)
                {
                    //TODO: wait until the block changes its state to allocated and put the next block to the result
                }
                else
                {
                    DynamicBlock* currentBlock = block->nextBlock;
                    DynamicBlock* firstBlock = currentBlock;
                    if(firstBlock)
                    {
                        acquireLock(firstBlock); //TODO: use RAII!
                        firstBlock->refCount++;
                        releaseLock(firstBlock);
                    }

                    while(currentBlock)
                    {
                        acquireLock(currentBlock); //TODO: use RAII!

                        if(DynamicBlockState::isFree(block->state))
                        {
                            accumulatedSize += currentBlock->size;
                            if(accumulatedSize >= lockedSize)
                            {
                                result.first = currentBlock->nextBlock;

                                acquireLock(firstBlock); //TODO: use RAII!
                                firstBlock->refCount--;
                                releaseLock(firstBlock);

                                releaseLock(currentBlock);
                                break;
                            }
                        }
                        else if(DynamicBlockState::isLocked(block->state))
                        {
                            //TODO: restart traverse(recursive)
                        }
                        else //block is allocated
                        {
                            result.first = currentBlock->nextBlock;

                            acquireLock(firstBlock); //TODO: use RAII!
                            firstBlock->refCount--;
                            releaseLock(firstBlock);

                            releaseLock(currentBlock);
                            break;
                        }

                        releaseLock(currentBlock);
                    }
                }
            }
            else //block is allocated
            {
                result.first = block->nextBlock;
            }

            releaseLock(block);

            return result;
        }

        static DynamicBlock* fastForwardLockedChain(DynamicBlock* lockedBlock)
        {
            DynamicBlock* freeBlock = nullptr;

            lockedBlock->refCount++; //TODO: use RAII

            while(lockedBlock)
            {
                const std::size_t lockedSize = lockedBlock->state;
                const std::size_t lockedOffset = getAlignedOffset(lockedBlock->alignPower, *lockedBlock);
                std::size_t accumulatedSize = lockedBlock->size - lockedOffset;

                DynamicBlock* currentBlock = lockedBlock->nextBlock;
                DynamicBlock* prevBlock = lockedBlock;

                while(accumulatedSize < lockedSize)
                {
                    if(currentBlock)
                    {
                        acquireLock(currentBlock);



                        releaseLock(currentBlock);
                    }
                    else
                    {
                        break;
                    }
                }
            }

            lockedBlock->refCount--;

            return freeBlock;
        }
    };
}