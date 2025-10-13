#include <dory/memory/allocators/segregationAllocator.h>

namespace dory::memory
{
    SegregationAllocator::SegregationAllocator(PageAllocator& blockAllocator, PageAllocator& hugeBlockAllocator) noexcept
        :_blockAllocator(blockAllocator),
        _hugeBlockAllocator(hugeBlockAllocator)
    {}
}
