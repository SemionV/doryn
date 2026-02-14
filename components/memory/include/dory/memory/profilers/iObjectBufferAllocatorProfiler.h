#pragma once

#include "../resources/memoryBlock.h"

namespace dory::memory::profilers
{
    class IObjectBufferAllocatorProfiler
    {
    public:
        virtual ~IObjectBufferAllocatorProfiler() = default;

        virtual void traceChunkAllocation(const MemoryBlock&) = 0;
        virtual void traceChunkFree(const MemoryBlock&) = 0;
        virtual void traceAllocation(void* ptr, std::size_t size) = 0;
        virtual void traceFree(void* ptr, std::size_t size) = 0;
    };
}