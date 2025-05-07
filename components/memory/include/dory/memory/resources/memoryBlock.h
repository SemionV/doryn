#pragma once

namespace dory::memory
{
    struct MemoryBlock
    {
        void* ptr {};
        std::size_t pagesCount {};
        std::size_t pageSize {};
        std::size_t commitedPagesCount {};
    };
}