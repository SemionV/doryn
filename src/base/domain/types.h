#pragma once

#include "base/doryExport.h"

namespace dory::domain
{
    enum UnitScale
    {
        Identity = 1,
        Milli = 1000,
        Micro = 1000000,
        Nano = 1000000000
    };

    struct DORY_API TimeSpan
    {
        long duration;
        UnitScale unitsPerSecond;

        TimeSpan():
            duration(0),
            unitsPerSecond(UnitScale::Milli)
        {
        };

        TimeSpan(UnitScale unitsPerSecond):
            duration(0),
            unitsPerSecond(unitsPerSecond)
        {
        };

        double ToMilliseconds() const
        {
            return (duration / (double)unitsPerSecond) * UnitScale::Milli;
        }
    };

    class DORY_API TimeConverter
    {
        public:
            static double ToMilliseconds(TimeSpan timeSpan)
            {
                return (timeSpan.duration / (double)timeSpan.unitsPerSecond) * UnitScale::Milli;
            }
    };

    constexpr std::size_t getStringLength(const char* string)
    {
        std::size_t length = 0;
        if(string)
        {
            while(string[length] != 0)
            {
                ++length;
            }
        }

        return length;
    }

    template <std::string_view const&... Strs>
    struct JoinStrings
    {
        static constexpr auto impl() noexcept
        {
            constexpr std::size_t len = (Strs.size() + ... + 0);
            std::array<char, len + 1> arr{};
            auto append = [i = 0, &arr](auto const& s) mutable {
                for (auto c : s) arr[i++] = c;
            };
            (append(Strs), ...);
            arr[len] = 0;
            return arr;
        }
        static constexpr auto arr = impl();
        static constexpr std::string_view value {arr.data(), arr.size() - 1};
    };

    template <std::string_view const&... Strs>
    static constexpr auto JoinStringLiterals = JoinStrings<Strs...>::value;
}