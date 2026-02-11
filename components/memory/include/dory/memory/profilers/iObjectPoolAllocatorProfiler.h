#pragma once

#include "../resources/memoryBlock.h"

namespace dory::memory::profilers
{
    class IObjectPoolAllocatorProfiler
    {
    public:
        virtual ~IObjectPoolAllocatorProfiler() = default;

        virtual void traceChunkAllocation(const MemoryBlock&) = 0;
    };
}