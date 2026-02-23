#include <blockAllocationProfiler.h>

namespace dory::test_utilities
{
    void BlockAllocationProfiler::traceBlockAllocation(void* ptr, std::size_t size, std::size_t alignment, LabelType label)
    {
        blockAllocations.push_back(BlockAllocation {
            ptr, size, alignment, label
        });
    }

    void BlockAllocationProfiler::traceBlockFree(void* ptr, std::size_t size, std::size_t alignment)
    {
        blockFrees.push_back(BlockFree {
            ptr, size, alignment
        });
    }
}
