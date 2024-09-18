#pragma once

#include <cstddef>

namespace dory::generics
{
    template <typename... Types>
    struct TypeList {};

    template <typename TList>
    struct Length;

    template <typename... Types>
    struct Length<TypeList<Types...>> {
        static constexpr std::size_t value = sizeof...(Types);
    };
}