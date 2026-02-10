#pragma once

namespace dory::memory::profilers
{
    class IObjectPoolAllocatorProfiler
    {
    public:
        virtual ~IObjectPoolAllocatorProfiler() = default;

        virtual void traceChunkAllocation(std::size_t chunkSize, std::size_t pageCount) = 0;
    };
}