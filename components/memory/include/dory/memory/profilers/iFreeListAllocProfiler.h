#pragma once

#include "../resources/memoryBlock.h"

namespace dory::memory::profilers
{
    class IFreeListAllocProfiler
    {
    public:
        virtual ~IFreeListAllocProfiler() = default;

        virtual void traceChunkAlloc(void* ptr, std::size_t size) = 0;
        virtual void traceChunkChained(void* ptr) = 0;
    };
}