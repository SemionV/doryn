#pragma once

#include "dependencies.h"

namespace dory
{
    struct TypeMap
    {
        //TODO...
    };

    template<typename T, int Idx>
    struct TrivialMembersType;

    template<typename T, bool isTrivialTypeMember, bool isRoot>
    struct MemberTrivialType
    {
        using Type = T;
    };

    template<typename T, bool isTrivialTypeMember>
    struct MemberTrivialType<T, isTrivialTypeMember, true>
    {
        using Type = void;
    };

    template<typename T>
    struct MemberTrivialType<T, false, true>
    {
        using Type = typename TrivialMembersType<T, ((int)(refl::detail::type_info<T>::member_count) - 1)>::Type;
    };

    template<typename T>
    struct MemberTrivialType<T, false, false>: public MemberTrivialType<T, false, true>
    {};

    template<typename T, int Idx>
    struct TrivialMembersType
    {
        private:
            using MemberValueType = typename refl::detail::member_info<T, Idx>::value_type;
            using NestedMemberTrivialType = typename MemberTrivialType<MemberValueType, std::is_trivial_v<MemberValueType>, false>::Type;

        public:
            using Type = std::conditional_t<std::is_trivial_v<NestedMemberTrivialType>, 
                    NestedMemberTrivialType, 
                    typename TrivialMembersType<T, Idx - 1>::Type>;
    };

    template<typename T>
    struct TrivialMembersType<T, -1>
    {
        using Type = void;
    };
    
    template<typename T, int Idx>
    struct TypeMembersSize;

    template<typename T, bool isTrivialTypeMember>
    struct TypeSize
    {
        static const std::size_t value = sizeof(T);
    };

    template<typename T>
    struct TypeSize<T, false>
    {
        static const std::size_t value = TypeMembersSize<T, static_cast<int>(refl::detail::type_info<T>::member_count) - 1>::value;
    };

    template<typename T, int Idx>
    struct TypeMembersSize
    {
        private:
            using MemberValueType = typename refl::detail::member_info<T, Idx>::value_type;

        public:
            static const std::size_t value = TypeSize<MemberValueType, std::is_trivial_v<MemberValueType>>::value + 
                TypeMembersSize<T, Idx - 1>::value;
    };

    template<typename T>
    struct TypeMembersSize<T, -1>
    {
        public:
            static const std::size_t value = 0;
    };

    template<typename T, int Idx>
    struct TypeTrivialMembersCount;

    template<typename T, bool isTrivialTypeMember, bool isRoot>
    struct TypeCount
    {
        static const std::size_t value = 1;
    };

    template<typename T, bool isTrivialTypeMember>
    struct TypeCount<T, isTrivialTypeMember, true>
    {
        static const std::size_t value = 0;
    };

    template<typename T>
    struct TypeCount<T, false, true>
    {
        static const std::size_t value = TypeTrivialMembersCount<T, static_cast<int>(refl::detail::type_info<T>::member_count) - 1>::value;
    };

    template<typename T>
    struct TypeCount<T, false, false>: public TypeCount<T, false, true>
    {};

    template<typename T, int Idx>
    struct TypeTrivialMembersCount
    {
        private:
            using MemberValueType = typename refl::detail::member_info<T, Idx>::value_type;

        public:
            static const std::size_t value = TypeCount<MemberValueType, std::is_trivial_v<MemberValueType>, false>::value + 
                TypeTrivialMembersCount<T, Idx - 1>::value;
    };

    template<typename T>
    struct TypeTrivialMembersCount<T, -1>
    {
        public:
            static const std::size_t value = 0;
    };

    template<typename T>
    struct TypeDescriptor
    {
        static const bool isTrivial = std::is_trivial_v<T>;

        using Type = T;
        using TrivialMemberType = typename MemberTrivialType<T, isTrivial, true>::Type;

        static const std::size_t size = TypeSize<T, isTrivial>::value;
        static const std::size_t trivialMemberCount = TypeCount<T, isTrivial, true>::value;
    };

    template<auto Id, typename T>
    struct Attribute
    {
        static constexpr auto id = Id;
        using Type = T;
    };

    template <class... TAttributes>
    struct Layout;

    template<>
    struct Layout<>
    {
        static const std::size_t count = 0;

        struct Size
        {
            static const std::size_t value = 0;
        };

        template<auto attributeId, std::size_t offset>
        struct AttributeOffset
        {
            static const std::size_t value = 0;
        };

        template<auto attributeId>
        struct AttributeSize
        {
            static const std::size_t value = 0;
        };

        template<auto attributeId>
        struct AttributeTrivialMemberCount
        {
            static const std::size_t value = 0;
        };

        template<auto attributeId>
        struct AttributeTrivialMemberType
        {
            using Type = void;
        };

        template<auto attributeId>
        struct AttributeType
        {
            using Type = void;
        };
    };

    template<auto id, typename T, auto... ids, typename... Ts>
    struct Layout<Attribute<id, T>, Attribute<ids, Ts>...>: public Layout<Attribute<ids, Ts>...>
    {
        using ParentType = Layout<Attribute<ids, Ts>...>;
        using AttributeTypeDescriptor = TypeDescriptor<T>;

        static const std::size_t count = sizeof...(Ts) + 1;

        struct Size
        {
            static const std::size_t value = AttributeTypeDescriptor::size + ParentType::Size::value;
        };

        template<auto attributeId, std::size_t offset = 0>
        struct AttributeOffset
        {
            static const std::size_t value = attributeId == id ? 
                offset : ParentType::template AttributeOffset<attributeId, offset + AttributeTypeDescriptor::size>::value;
        };

        template<auto attributeId>
        struct AttributeSize
        {
            static const std::size_t value = attributeId == id ? 
                AttributeTypeDescriptor::size : ParentType::template AttributeSize<attributeId>::value;
        };

        template<auto attributeId>
        struct AttributeTrivialMemberCount
        {
            static const std::size_t value = attributeId == id ? 
                AttributeTypeDescriptor::trivialMemberCount : ParentType::template AttributeTrivialMemberCount<attributeId>::value;
        };

        template<auto attributeId>
        struct AttributeTrivialMemberType
        {
            using Type = std::conditional_t<attributeId == id,
                typename AttributeTypeDescriptor::TrivialMemberType,
                typename ParentType::AttributeTrivialMemberType<attributeId>::Type>;
        };

        template<auto attributeId>
        struct AttributeType
        {
            using Type = std::conditional_t<attributeId == id,
                typename AttributeTypeDescriptor::Type,
                typename ParentType::AttributeType<attributeId>::Type>;
        };

        template<auto attributeId>
        struct Attribute
        {
            using TrivialMemberType = typename AttributeTrivialMemberType<attributeId>::Type;
            using Type = typename AttributeType<attributeId>::Type;
            static const std::size_t size = AttributeSize<attributeId>::value;
            static const std::size_t trivialMemberCount = AttributeTrivialMemberCount<attributeId>::value;
            static const std::size_t offset = AttributeOffset<attributeId>::value;
        };
    };

    template<typename T>
    struct LayoutSize
    {
        static const std::size_t value = 0;
    };

    template<auto id, typename T, auto... ids, typename... Ts>
    struct LayoutSize<Layout<Attribute<id, T>, Attribute<ids, Ts>...>>
    {
        static const std::size_t value = TypeSize<T, std::is_trivial_v<T>>::value + 
            LayoutSize<Layout<Attribute<ids, Ts>...>>::value;
    };

    template<>
    struct LayoutSize<Layout<>>
    {
        static const std::size_t value = 0;
    };

    template<typename T>
    static const auto LayoutSize_v = LayoutSize<T>::value;

    template<auto attributeId, typename T>
    struct LayoutAttributeSize
    {
        static const std::size_t value = 0;
    };

    template<auto attributeId, typename T, typename... Ts>
    struct LayoutAttributeSize<attributeId, Layout<T, Ts...>>
    {
        using AttributeType = typename T::Type;

        static const std::size_t value = attributeId == T::id ? 
            TypeSize<AttributeType, std::is_trivial_v<AttributeType>>::value 
            : LayoutAttributeSize<attributeId, Layout<Ts...>>::value;
    };

    template<auto attributeId>
    struct LayoutAttributeSize<attributeId, Layout<>>
    {
        static const std::size_t value = 0;
    };

    template<auto attributeId, typename T>
    static const auto LayoutAttributeSize_v = LayoutAttributeSize<attributeId, T>::value;

    template<auto attributeId, typename T>
    struct LayoutAttributeType
    {
        using Type = void;
    };

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
    using LayoutAttributeType_t = typename LayoutAttributeType<attributeId, T>::Type;
}