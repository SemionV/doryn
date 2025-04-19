#include <dory/memory/allocators/dynamicAllocator.h>

namespace dory::memory
{
    DynamicAllocator::DynamicAllocator(const MemoryBlock& memoryBlock,
        PoolAllocator<BlockDescriptor>& descriptorsAllocator, PoolAllocator<AddressHandle>& handlesAllocator) noexcept:
        _memory(memoryBlock),
        _descriptorsAllocator(descriptorsAllocator),
        _handlesAllocator(handlesAllocator)
    {
        _headDescriptor = _descriptorsAllocator.allocate();
        assert::debug(_headDescriptor, "Cannot allocate the head block descriptor");
        _headDescriptor->offset = 0;
        _headDescriptor->size = 0;

        _headDescriptor->nextDescriptor = _descriptorsAllocator.allocate();
        _headDescriptor->nextDescriptor->offset = 0;
        _headDescriptor->nextDescriptor->size = _memory.size;
    }
}
