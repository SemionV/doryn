#include <dory/memory/profilers/objectPoolAllocationProfiler.h>

namespace dory::memory::profilers
{
    void ObjectPoolAllocationProfiler::traceChunkAllocation(const MemoryBlock& memoryBlock)
    {
        ++chunksAllocated;
        memoryAllocated +=  memoryBlock.pagesCount * memoryBlock.pageSize;
    }
}
