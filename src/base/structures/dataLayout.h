#pragma once

#include "base/reflection.h"
#include "typeMap.h"

namespace dory::dataLayout
{
    template<auto Id, typename T>
    struct Attribute
    {
        static constexpr auto id = Id;
        using Type = T;
    };

    template<typename... Ts>
    struct Layout
    {
    };

    template<typename T>
    struct LayoutCount;

    template<typename... Ts>
    struct LayoutCount<Layout<Ts...>>
    {
        static constexpr std::size_t value = sizeof...(Ts);
    };

    template<typename T>
    static constexpr auto LayoutCountV = LayoutCount<T>::value;

    template<typename T>
    struct LayoutSize;

    template<typename T>
    static constexpr auto LayoutSizeV = LayoutSize<T>::value;

    template<auto id, typename T, auto... ids, typename... Ts>
    struct LayoutSize<Layout<Attribute<id, T>, Attribute<ids, Ts>...>>
    {
        static constexpr std::size_t value = reflection::TypeSizeV<T> + LayoutSizeV<Layout<Attribute<ids, Ts>...>>;
    };

    template<>
    struct LayoutSize<Layout<>>
    {
        static constexpr std::size_t value = 0;
    };

    template<auto attributeId, typename T>
    struct LayoutAttributeSize;

    template<auto attributeId, typename T>
    static constexpr auto LayoutAttributeSizeV = LayoutAttributeSize<attributeId, T>::value;

    template<auto attributeId, typename T, typename... Ts>
    struct LayoutAttributeSize<attributeId, Layout<T, Ts...>>
    {
        using AttributeType = typename T::Type;

        static constexpr std::size_t value = attributeId == T::id ?
                                         reflection::TypeSizeV<AttributeType>
                                        : LayoutAttributeSizeV<attributeId, Layout<Ts...>>;
    };

    template<auto attributeId>
    struct LayoutAttributeSize<attributeId, Layout<>>
    {
        static constexpr std::size_t value = 0;
    };

    template<auto attributeId, typename T>
    struct LayoutAttributeType;

    template<auto attributeId, typename T, typename... Ts>
    struct LayoutAttributeType<attributeId, Layout<T, Ts...>>
    {
        using ChoosenType = std::conditional_t<attributeId == T::id,
                T, LayoutAttributeType<attributeId, Layout<Ts...>>>;

        using Type = ChoosenType::Type;
    };

    template<auto attributeId>
    struct LayoutAttributeType<attributeId, Layout<>>
    {
        using Type = void;
    };

    template<auto attributeId, typename T>
    using LayoutAttributeTypeT = typename LayoutAttributeType<attributeId, T>::Type;

    template<auto attributeId, typename T>
    struct LayoutAttributeFieldType;

    template<auto attributeId, typename T, typename... Ts>
    struct LayoutAttributeFieldType<attributeId, Layout<T, Ts...>>
    {
        using AttributeType = typename T::Type;

        using ChoosenType = std::conditional_t<attributeId == T::id,
        reflection::MemberFieldTrivialType<AttributeType, true>,
        LayoutAttributeFieldType<attributeId, Layout<Ts...>>>;

        using Type = ChoosenType::Type;
    };

    template<auto attributeId>
    struct LayoutAttributeFieldType<attributeId, Layout<>>
    {
        using Type = void;
    };

    template<auto attributeId, typename T>
    using LayoutAttributeFieldTypeT = typename LayoutAttributeFieldType<attributeId, T>::Type;

    template<auto attributeId, typename T>
    struct LayoutAttributeFieldCount;

    template<auto attributeId, typename T>
    static constexpr auto LayoutAttributeFieldCountV = LayoutAttributeFieldCount<attributeId, T>::value;

    template<auto attributeId, typename T, typename... Ts>
    struct LayoutAttributeFieldCount<attributeId, Layout<T, Ts...>>
    {
        using AttributeType = typename T::Type;

        static constexpr std::size_t value = attributeId == T::id ?
                                             reflection::TypeCount<AttributeType, true>::value
                                             : LayoutAttributeFieldCountV<attributeId, Layout<Ts...>>;
    };

    template<auto attributeId>
    struct LayoutAttributeFieldCount<attributeId, Layout<>>
    {
        static constexpr std::size_t value = 0;
    };

    template<auto attributeId, typename T, std::size_t offset = 0>
    struct LayoutAttributeOffset;

    template<auto attributeId, typename T, std::size_t offset = 0>
    static constexpr auto LayoutAttributeOffsetV = LayoutAttributeOffset<attributeId, T, offset>::value;

    template<auto attributeId, typename T, typename... Ts, std::size_t offset>
    struct LayoutAttributeOffset<attributeId, Layout<T, Ts...>, offset>
    {
        using AttributeType = typename T::Type;

        static constexpr std::size_t value = attributeId == T::id ?
                     offset
                     : LayoutAttributeOffsetV<attributeId, Layout<Ts...>, offset + reflection::TypeSizeV<AttributeType>>;
    };

    template<auto attributeId, std::size_t offset>
    struct LayoutAttributeOffset<attributeId, Layout<>, offset>
    {
        static constexpr std::size_t value = 0;
    };
}