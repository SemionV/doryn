#include <dory/memory/allocators/segregationAllocator.h>

namespace dory::memory
{
    SegregationAllocator::SegregationAllocator(BlockAllocator& blockAllocator, BlockAllocator& hugeBlockAllocator) noexcept
        :_blockAllocator(blockAllocator),
        _hugeBlockAllocator(hugeBlockAllocator)
    {}
}
