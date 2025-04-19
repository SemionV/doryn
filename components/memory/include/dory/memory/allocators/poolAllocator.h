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

        T* allocate()
        {
            const std::size_t index = _freeList.getLeastUnsetBitIndex();
            if(index != BitArray<std::uint64_t>::npos)
            {
                _freeList.set(index);
                return &_memoryPool.pointer[index];
            }

            return nullptr;
        }

        void deallocate(const T* ptr)
        {
            assert::debug(ptr, "Invalid pointer");
            const std::ptrdiff_t diff = ptr - _memoryPool.pointer;
            const std::size_t index = diff / sizeof(T);
            _freeList.clear(index);
        }

        void reset()
        {
            _freeList.clearAll();
        }
    };
}