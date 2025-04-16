#pragma once

namespace dory::memory
{
    template<typename T>
    struct MemoryPool
    {
        using ItemType = T;
        std::size_t itemsCount {};
        ItemType* pointer {};
    };
}