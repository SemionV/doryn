#include <dory/memory/profilers/objectBufferAllocationProfiler.h>

namespace dory::memory::profilers
{
    void ObjectBufferAllocationProfiler::traceChunkAllocation(const MemoryBlock& memoryBlock)
    {
        ++chunksAllocated;
        memoryAllocated +=  memoryBlock.pagesCount * memoryBlock.pageSize;
    }

    void ObjectBufferAllocationProfiler::traceChunkFree(const MemoryBlock& memoryBlock)
    {
        --chunksFreed;
        memoryFreed += memoryBlock.pagesCount * memoryBlock.pageSize;
    }

    void ObjectBufferAllocationProfiler::traceAllocation(void* ptr, const std::size_t size)
    {
        ++objectsAllocated;
        objectsMemoryAllocated += size;
    }

    void ObjectBufferAllocationProfiler::traceFree(void* ptr, const std::size_t size)
    {
        ++objectsFreed;
        objectsMemoryFreed += size;
    }
}
