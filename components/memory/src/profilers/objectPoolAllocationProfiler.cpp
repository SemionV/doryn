#include <dory/memory/profilers/objectPoolAllocationProfiler.h>

namespace dory::memory::profilers
{
    void ObjectPoolAllocationProfiler::traceChunkAllocation(const MemoryBlock& memoryBlock)
    {
        ++chunksAllocated;
        memoryAllocated +=  memoryBlock.pagesCount * memoryBlock.pageSize;
    }

    void ObjectPoolAllocationProfiler::traceChunkFree(const MemoryBlock& memoryBlock)
    {
        --chunksFreed;
        memoryFreed += memoryBlock.pagesCount * memoryBlock.pageSize;
    }

    void ObjectPoolAllocationProfiler::traceAllocation(void* ptr, const std::size_t size)
    {
        ++objectsAllocated;
        objectsMemoryAllocated += size;
    }

    void ObjectPoolAllocationProfiler::traceFree(void* ptr, const std::size_t size)
    {
        ++objectsFreed;
        objectsMemoryFreed += size;
    }
}
