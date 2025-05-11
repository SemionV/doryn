#include <cstring>
#include <dory/memory/allocators/bumpAllocator.h>
#include <dory/memory/allocation.h>

namespace dory::memory
{
    BumpAllocator::BumpAllocator(MemoryBlock& memoryBlock)
        :_memoryBlock(memoryBlock),
        _free(reinterpret_cast<std::uintptr_t>(memoryBlock.ptr)),
        _size(memoryBlock.pageSize * memoryBlock.pagesCount)
    {}

    void* BumpAllocator::allocate(const std::size_t size, const std::uint8_t align) noexcept
    {
        const std::uintptr_t resultAddress = alignAddress(_free, 1 << align);
        const std::size_t newFreeAddress = resultAddress + size;
        if(newFreeAddress > _size)
        {
            return nullptr;
        }
        _free = newFreeAddress;
        return reinterpret_cast<void*>(resultAddress);
    }

    void BumpAllocator::deallocate(void* ptr)
    {}

    std::size_t BumpAllocator::getAllocatedSize() const
    {
        return _free - reinterpret_cast<std::uintptr_t>(_memoryBlock.ptr);
    }
}
