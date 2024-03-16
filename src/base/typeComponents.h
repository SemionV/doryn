#pragma once

#include "dependencies.h"

namespace dory
{
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

    class Uncopyable
    {
    public:
        Uncopyable(const Uncopyable&) = delete;
        Uncopyable& operator=(const Uncopyable&) = delete;

        Uncopyable() = default;
    };

    template<typename TImpelementation>
    class StaticInterface
    {
    protected:
        TImpelementation* toImplementation()
        {
            return static_cast<TImpelementation*>(this);
        }
    };
}