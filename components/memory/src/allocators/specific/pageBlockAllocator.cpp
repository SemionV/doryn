#include <dory/memory/allocators/specific/pageBlockAllocator.h>
#include <dory/memory/allocation.h>

namespace dory::memory::allocators::specific
{
    PageBlockAllocator::PageBlockAllocator(const std::size_t pageSize, profilers::IBlockAllocationProfiler* profiler):
        BlockAllocator(profiler),
        _pageSize(pageSize)
    {}

    void* PageBlockAllocator::allocateBlockImpl(LabelType label, std::size_t multiple) const
    {
        return reserveMemoryPages(_pageSize, multiple);
    }

    void PageBlockAllocator::deallocateBlockImpl(void* ptr, std::size_t multiple) const
    {
        releaseMemoryPages(ptr, _pageSize, multiple);
    }

    std::size_t PageBlockAllocator::getBlockSizeImpl() const
    {
        return _pageSize;
    }
}
