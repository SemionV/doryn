#pragma once
#include <type_traits>

namespace dory::bitwise
{
    template <typename T>
    T nextPowerOfTwo(T x)
    {
        static_assert(std::is_unsigned_v<T>, "T must be unsigned");
        if (x == 0) return 1;

        --x;
        for (std::size_t i = 1; i < sizeof(T) * 8; i <<= 1)
            x |= x >> i;

        return x + 1;
    }
}
