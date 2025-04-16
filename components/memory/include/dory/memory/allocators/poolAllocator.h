#pragma once

#include <dory/macros/assert.h>
#include <dory/memory/bitArray.h>
#include <dory/memory/resources/memoryPool.h>

namespace dory::memory
{
    template<typename T>
    class PoolAllocator
    {
    private:
        MemoryPool<T> _memoryPool;
        BitArray<std::uint64_t> _freeList;

    public:
        explicit PoolAllocator(const MemoryPool<T>& memoryPool, const BitArray<std::uint64_t>& freeList) noexcept:
            _memoryPool(memoryPool),
            _freeList(freeList)
        {}

        std::size_t allocate()
        {
            const std::size_t index = _freeList.getLeastUnsetBitIndex();
            if(index != BitArray<std::uint64_t>::npos)
            {
                _freeList.set(index);
            }

            return index;
        }

        void deallocate(const std::size_t index)
        {
            _freeList.clear(index);
        }

        T* getAddress(std::size_t index) const
        {
            if(index < _memoryPool.itemsCount)
            {
                return &_memoryPool.pointer[index];
            }
            assert::debug(false, "Invalid item index");
            return nullptr;
        }

        void reset()
        {
            _freeList.clearAll();
        }
    };
}