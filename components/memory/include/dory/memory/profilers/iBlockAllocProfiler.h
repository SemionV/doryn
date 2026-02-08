#pragma once

namespace dory::memory::profilers
{
    class IBlockAllocProfiler
    {
    public:
        virtual ~IBlockAllocProfiler() = default;

        virtual void traceBlockAlloc(void* ptr, std::size_t size) = 0;
        virtual void traceBlocAllocFailure(std::size_t size) = 0;
        virtual void traceBlockFree(void* ptr, std::size_t size) = 0;
        virtual void traceBlockFree(void* ptr) = 0;
    };
}
