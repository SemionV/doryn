#pragma once

#include <dory/memory/profilers/iAllocatorProfiler.h>

namespace dory::test_utilities
{
    class Profiler: public memory::profilers::IAllocatorProfiler
    {
    public:
        struct AllocationCounters
        {
            std::size_t sizeAllocated = 0;
            std::size_t sizeFreed = 0;

            std::size_t allocatedCount = 0;
            std::size_t freedCount = 0;
        };

        AllocationCounters bytesCounters;
        AllocationCounters objectCounters;
        AllocationCounters arrayCounters;

    public:
        void traceBytesAllocation(void* ptr, size_t size, size_t alignment, LabelType label) final;
        void traceBytesFree(void* ptr, size_t size, size_t alignment) final;
        void traceObjectAllocation(void* ptr, size_t size, size_t alignment, LabelType label) final;
        void traceObjectFree(void* ptr, size_t size, size_t alignment) final;
        void traceArrayAllocation(void* ptr, std::size_t count, size_t itemSize, size_t itemAlignment, LabelType label) final;
        void traceArrayFree(void* ptr, std::size_t count, size_t itemSize, size_t itemAlignment) final;
    };
}