#pragma once

#include "iObjectPoolAllocatorProfiler.h"

namespace dory::memory::profilers
{
    class ObjectPoolAllocationProfiler: public IObjectPoolAllocatorProfiler
    {
    public:
        std::size_t chunksAllocated {};
        std::size_t chunksFreed {};
        std::size_t memoryAllocated {};
        std::size_t memoryFreed {};
        std::size_t objectsAllocated {};

    public:
        void traceChunkAllocation(const MemoryBlock&) final;
        void traceAllocation(void* ptr) final;
        void traceChunkFree(const MemoryBlock&) final;
    };
}