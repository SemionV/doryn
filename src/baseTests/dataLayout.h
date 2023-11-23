#pragma once

#include "dependencies.h"
#include "reflection.h"

namespace dory
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

    template<auto id, typename T, auto... ids, typename... Ts>
    struct LayoutSize<Layout<Attribute<id, T>, Attribute<ids, Ts>...>>
    {
        static constexpr std::size_t value = TypeSize<T, std::is_trivial_v<T>>::value + 
            LayoutSize<Layout<Attribute<ids, Ts>...>>::value;
    };

    template<>
    struct LayoutSize<Layout<>>
    {
        static constexpr std::size_t value = 0;
    };

    template<typename T>
    static constexpr auto LayoutSizeV = LayoutSize<T>::value;

    template<auto attributeId, typename T>
    struct LayoutAttributeSize;

    template<auto attributeId, typename T, typename... Ts>
    struct LayoutAttributeSize<attributeId, Layout<T, Ts...>>
    {
        using AttributeType = typename T::Type;

        static constexpr std::size_t value = attributeId == T::id ? 
            TypeSize<AttributeType, std::is_trivial_v<AttributeType>>::value 
            : LayoutAttributeSize<attributeId, Layout<Ts...>>::value;
    };

    template<auto attributeId>
    struct LayoutAttributeSize<attributeId, Layout<>>
    {
        static constexpr std::size_t value = 0;
    };

    template<auto attributeId, typename T>
    static constexpr auto LayoutAttributeSizeV = LayoutAttributeSize<attributeId, T>::value;

    template<auto attributeId, typename T>
    struct LayoutAttributeType;

    template<auto attributeId, typename T, typename... Ts>
    struct LayoutAttributeType<attributeId, Layout<T, Ts...>>
    {
        using AttributeType = typename T::Type;

        using Type = std::conditional_t<attributeId == T::id,
            AttributeType, typename LayoutAttributeType<attributeId, Layout<Ts...>>::Type>;
    };

    template<auto attributeId>
    struct LayoutAttributeType<attributeId, Layout<>>
    {
        using Type = void;
    };

    template<auto attributeId, typename T>
    using LayoutAttributeTypeT = typename LayoutAttributeType<attributeId, T>::Type;

    template<auto attributeId, typename T>
    struct LayoutAttributeMemberType;

    template<auto attributeId, typename T, typename... Ts>
    struct LayoutAttributeMemberType<attributeId, Layout<T, Ts...>>
    {
        using AttributeType = typename T::Type;

        using Type = std::conditional_t<attributeId == T::id,
            typename MemberTrivialType<AttributeType, std::is_trivial_v<AttributeType>, true>::Type, 
            typename LayoutAttributeMemberType<attributeId, Layout<Ts...>>::Type>;
    };

    template<auto attributeId>
    struct LayoutAttributeMemberType<attributeId, Layout<>>
    {
        using Type = void;
    };

    template<auto attributeId, typename T>
    using LayoutAttributeMemberTypeT = typename LayoutAttributeMemberType<attributeId, T>::Type;

    template<auto attributeId, typename T>
    struct LayoutAttributeMemberCount;

    template<auto attributeId, typename T, typename... Ts>
    struct LayoutAttributeMemberCount<attributeId, Layout<T, Ts...>>
    {
        using AttributeType = typename T::Type;

        static constexpr std::size_t value = attributeId == T::id ? 
            TypeCount<AttributeType, std::is_trivial_v<AttributeType>, true>::value 
            : LayoutAttributeMemberCount<attributeId, Layout<Ts...>>::value;
    };

    template<auto attributeId>
    struct LayoutAttributeMemberCount<attributeId, Layout<>>
    {
        static constexpr std::size_t value = 0;
    };

    template<auto attributeId, typename T>
    static constexpr auto LayoutAttributeMemberCountV = LayoutAttributeMemberCount<attributeId, T>::value;

    template<auto attributeId, typename T, std::size_t offset = 0>
    struct LayoutAttributeOffset;

    template<auto attributeId, typename T, typename... Ts, std::size_t offset>
    struct LayoutAttributeOffset<attributeId, Layout<T, Ts...>, offset>
    {
        using AttributeType = typename T::Type;

        static constexpr std::size_t value = attributeId == T::id ? 
            offset 
            : LayoutAttributeOffset<attributeId, Layout<Ts...>, offset + TypeSize<AttributeType, std::is_trivial_v<AttributeType>>::value>::value;
    };

    template<auto attributeId, std::size_t offset>
    struct LayoutAttributeOffset<attributeId, Layout<>, offset>
    {
        static constexpr std::size_t value = 0;
    };

    template<auto attributeId, typename T>
    static constexpr auto LayoutAttributeOffsetV = LayoutAttributeOffset<attributeId, T>::value;
}