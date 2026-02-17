#pragma once

#include <dory/types.h>
#include <dory/interface.h>

namespace dory::memory::profilers
{
    class IAllocatorProfiler: public Interface
    {
    public:
        virtual void traceBytesAllocation(void* ptr, size_t size, size_t alignment, LabelType label) = 0;
        virtual void traceBytesFree(void* ptr, size_t size, size_t alignment) = 0;

        virtual void traceObjectAllocation(void* ptr, size_t size, size_t alignment, LabelType label) = 0;
        virtual void traceObjectFree(void* ptr, size_t size, size_t alignment) = 0;

        virtual void traceArrayAllocation(void* ptr, std::size_t count, size_t itemSize, size_t itemAlignment, LabelType label) = 0;
        virtual void traceArrayFree(void* ptr, std::size_t count, size_t itemSize, size_t itemAlignment) = 0;
    };
}