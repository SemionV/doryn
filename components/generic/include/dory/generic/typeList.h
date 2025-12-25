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

        static int get(TValue searchValue)
        {
            if(searchValue == Value)
            {
                return Index;
            }

            return ValueIndexTraverse<Index + 1, TValue, SearchValue, List...>::get(searchValue);
        }
    };

    template <std::size_t Index, typename TValue, TValue SearchValue>
    struct ValueIndexTraverse<Index, TValue, SearchValue>
    {
        static constexpr int value = -1;

        static int get(TValue searchValue)
        {
            return -1;
        }
    };

    template <std::size_t Index, typename TValue, TValue... List>
    struct ValueIndexTraverseRuntime;

    template <std::size_t Index, typename TValue, TValue Value, TValue... List>
    struct ValueIndexTraverseRuntime<Index, TValue, Value, List...>
    {
        static int get(TValue searchValue)
        {
            if(searchValue == Value)
            {
                return Index;
            }

            return ValueIndexTraverseRuntime<Index + 1, TValue, List...>::get(searchValue);
        }
    };

    template <std::size_t Index, typename TValue>
    struct ValueIndexTraverseRuntime<Index, TValue>
    {
        static int get(TValue searchValue)
        {
            return -1;
        }
    };

    template<auto SearchValue, typename TValueList>
    struct ValueIndex;

    template<typename TValue, TValue SearchValue, TValue... Values>
    struct ValueIndex<SearchValue, ValueList<TValue, Values...>>
    {
        static constexpr int value = ValueIndexTraverse<0, TValue, SearchValue, Values...>::value;

        static int get(TValue searchValue)
        {
            return ValueIndexTraverse<0, TValue, SearchValue, Values...>::get(searchValue);
        }
    };

    template<typename TValueList>
    struct ValueIndexRuntime;

    template<typename TValue, TValue... Values>
    struct ValueIndexRuntime<ValueList<TValue, Values...>>
    {
        static int get(TValue searchValue)
        {
            return ValueIndexTraverseRuntime<0, TValue, Values...>::get(searchValue);
        }
    };

    /*template <std::size_t Index, typename TValue, TValue... List>
    struct ValueArrayTraverse;

    template <std::size_t Index, typename TValue, TValue Value, TValue... List>
    struct ValueArrayTraverse<Index, TValue, Value, List...>
    {
        static constexpr void setValue(TValue* array)
        {
            array[Index] = Value;
            return ValueArrayTraverse<Index + 1, TValue, List...>::setValue(array);
        }
    };

    template <std::size_t Index, typename TValue>
    struct ValueArrayTraverse<Index, TValue>
    {
        static constexpr void setValue(TValue* array)
        {}
    };*/

    template<typename TValueList>
    struct ValueArray;

    template<typename TValue, TValue... Values>
    struct ValueArray<ValueList<TValue, Values...>>
    {
        static constexpr auto get()
        {
            return std::array<TValue, ValueListLength<ValueList<TValue, Values...>>::value>{ Values... };
        }
    };
}