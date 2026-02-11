#pragma once

#include "iObjectPoolAllocatorProfiler.h"

namespace dory::memory::profilers
{
    class ObjectPoolAllocationProfiler: public IObjectPoolAllocatorProfiler
    {
    public:
        std::size_t chunksAllocated {};
        std::size_t     memoryAllocated {};

    public:
        void traceChunkAllocation(const MemoryBlock&) final;
    };
}