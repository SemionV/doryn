#include <dory/memory/allocators/dynamicAllocator.h>

namespace dory::memory
{
    DynamicAllocator::DynamicAllocator(const MemoryBlock& memoryBlock) noexcept:
        _memory(memoryBlock)
    {}
}
