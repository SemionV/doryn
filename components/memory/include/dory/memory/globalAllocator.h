#pragma once
#include <cstddef>

namespace dory::memory
{
    struct MemoryAllocationCounters
    {
        std::size_t allocationsCount {};
        std::size_t deallocationsCount {};
        std::size_t allocatedBytesCount {};
        std::size_t deallocatedBytesCount {};
    };
}
