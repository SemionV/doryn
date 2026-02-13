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
        std::size_t objectsFreed {};
        std::size_t objectsMemoryAllocated {};
        std::size_t objectsMemoryFreed {};

    public:
        void traceChunkAllocation(const MemoryBlock&) final;
        void traceChunkFree(const MemoryBlock&) final;
        void traceAllocation(void* ptr, std::size_t size) final;
        void traceFree(void* ptr, std::size_t size) final;
    };
}