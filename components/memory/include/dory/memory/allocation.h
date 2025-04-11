#pragma once
#include <cstddef>
#include <dory/macros/assert.h>

namespace dory::memory
{
    inline std::size_t alignAddress(const std::size_t address, const std::size_t align)
    {
        const std::size_t mask = align - 1;
        assert::debug((align & mask) == 0, "Alignment must be a power of 2");
        return (address + mask) & ~mask;
    }

    template<typename T>
    T* alignPointer(T* ptr, std::size_t align)
    {
        const auto address = reinterpret_cast<std::size_t>(ptr);
        const auto alignedAddress = alignAddress(address, align);
        return reinterpret_cast<T*>(alignedAddress);
    }
}