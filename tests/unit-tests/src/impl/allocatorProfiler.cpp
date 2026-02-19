#include <allocatorProfiler.h>

namespace dory::test_utilities
{
    void Profiler::traceBytesAllocation(void* ptr, size_t size, size_t alignment, LabelType label)
    {
        bytesCounters.sizeAllocated += size;
        ++bytesCounters.allocatedCount;
    }

    void Profiler::traceBytesFree(void* ptr, size_t size, size_t alignment)
    {
        bytesCounters.sizeFreed += size;
        ++bytesCounters.freedCount;
    }

    void Profiler::traceObjectAllocation(void* ptr, size_t size, size_t alignment, LabelType label)
    {
        objectCounters.sizeAllocated += size;
        ++objectCounters.allocatedCount;
    }

    void Profiler::traceObjectFree(void* ptr, size_t size, size_t alignment)
    {
        objectCounters.sizeFreed += size;
        ++objectCounters.freedCount;
    }

    void Profiler::traceArrayAllocation(void* ptr, std::size_t count, size_t itemSize, size_t itemAlignment, LabelType label)
    {
        arrayCounters.sizeAllocated += count * itemSize;
        ++arrayCounters.allocatedCount;
    }

    void Profiler::traceArrayFree(void* ptr, std::size_t count, size_t itemSize, size_t itemAlignment)
    {
        arrayCounters.sizeFreed += count * itemSize;
        ++arrayCounters.freedCount;
    }
}
