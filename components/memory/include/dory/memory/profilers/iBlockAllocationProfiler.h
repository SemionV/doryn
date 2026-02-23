 #pragma once

#include <cstddef>
#include <dory/interface.h>
#include <dory/types.h>

 namespace dory::memory::profilers
{
    class IBlockAllocationProfiler: public Interface
    {
    public:
        virtual void traceBlockAllocation(void* ptr, std::size_t size, std::size_t alignment, LabelType label) = 0;
        virtual void traceBlockFree(void* ptr, std::size_t size, std::size_t alignment) = 0;
    };
}
