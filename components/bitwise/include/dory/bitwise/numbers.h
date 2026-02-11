#pragma once
#include <type_traits>

namespace dory::bitwise
{
    template <typename T>
    constexpr T nextPowerOfTwo(T x)
    {
        static_assert(std::is_unsigned_v<T>, "T must be unsigned");
        if (x == 0) return 1;

        --x;
        for (std::size_t i = 1; i < sizeof(T) * 8; i <<= 1)
            x |= x >> i;

        return x + 1;
    }

    template<typename T>
    requires std::is_integral_v<T>
    constexpr T log2Ceil(T x) noexcept
    {
        //following is optimized version of: return x <= 1 ? 0 : std::bit_width(x - 1);

        // bit_width(x - 1) is undefined for x == 0, so we fix that branchlessly
        const std::size_t nonzero = (x != 0);           // 1 if x > 0, 0 if x == 0
        const std::size_t mask = 0 - nonzero;           // all bits set if x > 0, 0 otherwise
        const std::size_t safe = (x - 1) & mask;        // 0 if x == 0, x-1 otherwise
        const std::size_t w = std::bit_width(safe);
        // For x == 0 or 1, the result should be 0
        return w & (0 - (x > 1));                       // zero result if x <= 1
    }
}
