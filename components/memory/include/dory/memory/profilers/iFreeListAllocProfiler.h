#pragma once

#include "../resources/memoryBlock.h"

namespace dory::memory::profilers
{
    class IFreeListAllocProfiler
    {
    public:
        virtual ~IFreeListAllocProfiler() = default;

        virtual void traceChunkAlloc(const MemoryBlock& memoryBlock) = 0;
        virtual void traceChunkInitialized(const MemoryBlock& memoryBlock) = 0;
        virtual void traceChunkChained(const MemoryBlockNode& memoryBlockNode) = 0;
    };
}