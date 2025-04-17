#include <dory/memory/allocators/dynamicAllocator.h>

namespace dory::memory
{
    DynamicAllocator::DynamicAllocator(const MemoryBlock& memoryBlock, const AddressList addressHandles) noexcept:
        _memory(memoryBlock), _addressHandles(addressHandles)
    {}
}
