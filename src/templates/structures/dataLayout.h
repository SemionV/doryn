#pragma once

#include "templates/reflection.h"
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

    template<typename T, typename TTypeMap = void>
    struct LayoutSize;

    template<auto id, typename T, typename TTypeMap, auto... ids, typename... Ts>
    struct LayoutSize<Layout<Attribute<id, T>, Attribute<ids, Ts>...>, TTypeMap>
    {
        using MappedType = typeMap::MappedTypeT<T, TTypeMap>;

        static constexpr std::size_t value = reflection::TypeSize<MappedType>::value +
        LayoutSize<Layout<Attribute<ids, Ts>...>, TTypeMap>::value;
    };

    template<typename TTypeMap>
    struct LayoutSize<Layout<>, TTypeMap>
    {
        static constexpr std::size_t value = 0;
    };

    template<typename T, typename TTypeMap = void>
    static constexpr auto LayoutSizeV = LayoutSize<T, TTypeMap>::value;

    template<auto attributeId, typename T>
    struct LayoutAttributeSize;

    template<auto attributeId, typename T, typename... Ts>
    struct LayoutAttributeSize<attributeId, Layout<T, Ts...>>
    {
        using AttributeType = typename T::Type;

        static constexpr std::size_t value = attributeId == T::id ?
                                         reflection::TypeSize<AttributeType>::value
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
    struct LayoutAttributeMemberType;

    template<auto attributeId, typename T, typename... Ts>
    struct LayoutAttributeMemberType<attributeId, Layout<T, Ts...>>
    {
        using AttributeType = typename T::Type;

        using ChoosenType = std::conditional_t<attributeId == T::id,
        reflection::MemberTrivialType<AttributeType, true>,
        LayoutAttributeMemberType<attributeId, Layout<Ts...>>>;

        using Type = ChoosenType::Type;
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
                                             reflection::TypeCount<AttributeType, true>::value
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
                     : LayoutAttributeOffset<attributeId, Layout<Ts...>, offset + reflection::TypeSize<AttributeType>::value>::value;
    };

    template<auto attributeId, std::size_t offset>
    struct LayoutAttributeOffset<attributeId, Layout<>, offset>
    {
        static constexpr std::size_t value = 0;
    };

    template<auto attributeId, typename T>
    static constexpr auto LayoutAttributeOffsetV = LayoutAttributeOffset<attributeId, T>::value;
}