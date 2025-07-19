#pragma once
#include <cstddef>

namespace dory::generic
{
    /*
     * Fast mod function for the cases when modulo is power of 2
    */
    inline std::size_t mod2(const std::size_t value, const std::size_t modulo)
    {
        return value & (modulo - 1); //it is very well possible to compute (modulo - 1) part and use as a constant
    }
}