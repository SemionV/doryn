#include <allocatorBuilder.h>

namespace dory::test_utilities
{
    AllocatorBuilder::AllocatorBuilder(memory::profilers::IBlockAllocProfiler* blockAllocProfiler,
        memory::profilers::IAllocatorProfiler* largeObjectAllocProfiler,
        memory::profilers::IAllocatorProfiler* memoryBlockNodeAllocProfiler):
        _blockAllocator(PAGE_SIZE, blockAllocProfiler),
        _largeObjectAllocator(largeObjectAllocProfiler),
        _memoryBlockNodeAllocator(memoryBlockNodeAllocProfiler)
    {
    }

    std::shared_ptr<AllocatorBuilder::SegregationAllocatorType> AllocatorBuilder::build(memory::profilers::IAllocatorProfiler* profiler)
    {
        return std::make_shared<SegregationAllocatorType>(_blockAllocator, _largeObjectAllocator, _memoryBlockNodeAllocator, profiler, _sizeClasses);
    }
}
