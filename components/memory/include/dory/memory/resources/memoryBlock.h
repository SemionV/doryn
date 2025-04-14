#pragma once

namespace dory::memory
{
    struct MemoryBlock
    {
        void* ptr {};
        std::size_t size {};
    };
}