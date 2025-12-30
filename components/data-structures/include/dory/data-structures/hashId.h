#pragma once
#include <cstdint>
#include <string_view>

namespace dory::data_structures::hash
{
    constexpr std::uint64_t fnv_offset = 14695981039346656037ull;
    constexpr std::uint64_t fnv_prime  = 1099511628211ull;

    constexpr std::uint64_t fmix64(std::uint64_t x) noexcept
    {
        x ^= x >> 33;
        x *= 0xff51afd7ed558ccdULL;
        x ^= x >> 33;
        x *= 0xc4ceb9fe1a85ec53ULL;
        x ^= x >> 33;
        return x;
    }

    constexpr std::uint64_t hash(std::string_view s) noexcept
    {
        std::uint64_t h = fnv_offset;
        for (unsigned char c : s)
        {
            h ^= static_cast<std::uint64_t>(c);
            h *= fnv_prime;
        }
        return fmix64(h);
    }

    consteval std::uint64_t operator"" _id(const char* s, std::size_t n) noexcept
    {
        return hash({s, n});
    }
}
