#pragma once
#include <cstddef>
#include <dory/macros/assert.h>

namespace dory::memory
{
    inline std::size_t alignAddress(std::size_t address, std::size_t align)
    {
        const std::size_t mask = align - 1;
        //TODO: finish the implementation
    }
}
