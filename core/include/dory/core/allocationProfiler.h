#pragma once

#include <iostream>

namespace dory::core
{
    class AllocProfiler
    {
    public:
        void traceSlotAlloc(void* ptr, std::size_t size, std::size_t slotSize, std::size_t classIndex);
        void traceSlotFree(void* ptr, std::size_t slotSize, std::size_t classIndex);
        void traceLargeAlloc(void* ptr, std::size_t size);
        void traceLargeFree(void* ptr);
    };
}