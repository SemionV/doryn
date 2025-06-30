#include <dory/memory/allocators/dynamicAllocator.h>

namespace dory::memory
{
    DynamicAllocator::DynamicAllocator(const MemoryBlock& memoryBlock,
        PoolAllocator<DynamicBlock>& descriptorsAllocator, PoolAllocator<AddressHandle>& handlesAllocator) noexcept:
        _memory(memoryBlock),
        _blocksAllocator(descriptorsAllocator),
        _handlesAllocator(handlesAllocator)
    {
        _headBlock = _blocksAllocator.allocate();
        assert::debug(_headBlock, "Cannot allocate the head block descriptor");

        _headBlock->offset = 0;
        _headBlock->size = _memory.pagesCount * _memory.pageSize;
        _headBlock->state = DynamicBlockState::free;
    }
}
