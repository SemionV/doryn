#pragma once

#include <cstddef>
#include <type_traits>

namespace dory::generic
{
    template <typename... Types>
    struct TypeList {};

    template <typename TList>
    struct Length;

    template <typename... Types>
    struct Length<TypeList<Types...>>
    {
        static constexpr std::size_t value = sizeof...(Types);
    };

    template <std::size_t Index, typename TType, typename... TList>
    struct TypeIndexTraverse;

    template <std::size_t Index, typename TType, typename T, typename... TList>
    struct TypeIndexTraverse<Index, TType, T, TList...>
    {
        static constexpr int value = std::is_same_v<TType, T> ? Index : TypeIndexTraverse<Index + 1, TType, TList...>::value;
    };

    template <std::size_t Index, typename TType>
    struct TypeIndexTraverse<Index, TType>
    {
        static constexpr int value = -1;
    };

    template<typename TType, typename TList>
    struct TypeIndex;

    template<typename TType, typename... TList>
    struct TypeIndex<TType, TypeList<TList...>>
    {
        static constexpr int value = TypeIndexTraverse<0, TType, TList...>::value;
    };

    template<typename TValue, TValue... Values>
    struct ValueList
    {
        using ValueType = TValue;
    };

    template <typename TList>
    struct ValueListLength;

    template <typename TValue, TValue... Values>
    struct ValueListLength<ValueList<TValue, Values...>>
    {
        static constexpr std::size_t value = sizeof...(Values);
    };

    template <std::size_t Index, typename TValue, TValue SearchValue, TValue... List>
    struct ValueIndexTraverse;

    template <std::size_t Index, typename TValue, TValue SearchValue, TValue Value, TValue... List>
    struct ValueIndexTraverse<Index, TValue, SearchValue, Value, List...>
    {
        static constexpr int value = SearchValue == Value ? Index : ValueIndexTraverse<Index + 1, TValue, SearchValue, List...>::value;
    };

    template <std::size_t Index, typename TValue, TValue SearchValue>
    struct ValueIndexTraverse<Index, TValue, SearchValue>
    {
        static constexpr int value = -1;
    };

    template<auto SearchValue, typename TValueList>
    struct ValueIndex;

    template<typename TValue, TValue SearchValue, TValue... Values>
    struct ValueIndex<SearchValue, ValueList<TValue, Values...>>
    {
        static constexpr int value = ValueIndexTraverse<0, TValue, SearchValue, Values...>::value;
    };
}