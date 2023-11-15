#pragma once

#include "dependencies.h"

namespace dory
{
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
            using NestedMemberTrivialType = MemberTrivialType<MemberValueType, std::is_trivial_v<MemberValueType>, false>;

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
        using type = T;
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
        struct AttributeTypeById
        {
            using Type = void;
        };
    };

    template<typename TAttribute, typename... TAttributes>
    struct Layout<TAttribute, TAttributes...>: public Layout<TAttributes...>
    {
        using ParentType = Layout<TAttributes...>;
        using AttributeType = typename TAttribute::type;
        using AttributeTypeDescriptor = TypeDescriptor<AttributeType>;

        static const std::size_t count = sizeof...(TAttributes) + 1;

        template<auto attributeId>
        struct AttributeTypeReflection
        {
            static const auto value = attributeId == TAttribute::id ? 
                refl::descriptor::type_descriptor<AttributeType> {} : 
                ParentType::template AttributeTypeReflection<attributeId>::value;
        };

        struct Size
        {
            static const std::size_t value = AttributeTypeDescriptor::size + ParentType::Size::value;
        };

        template<auto attributeId, std::size_t offset = 0>
        struct AttributeOffset
        {
            static const std::size_t value = attributeId == TAttribute::id ? 
                offset : ParentType::template AttributeOffset<attributeId, offset + AttributeTypeDescriptor::size>::value;
        };

        template<auto attributeId>
        struct AttributeSize
        {
            static const std::size_t value = attributeId == TAttribute::id ? 
                AttributeTypeDescriptor::size : ParentType::template AttributeSize<attributeId>::value;
        };

        template<auto attributeId>
        struct AttributeTrivialMemberCount
        {
            static const std::size_t value = attributeId == TAttribute::id ? 
                AttributeTypeDescriptor::trivialMemberCount : ParentType::template AttributeTrivialMemberCount<attributeId>::value;
        };

        template<auto attributeId>
        struct AttributeTrivialMemberType
        {
            using Type = std::conditional_t<attributeId == TAttribute::id,
                typename AttributeTypeDescriptor::TrivialMemberType,
                typename ParentType::AttributeTrivialMemberType<attributeId>::Type>;
        };

        template<auto attributeId>
        struct AttributeTypeById
        {
            using Type = std::conditional_t<attributeId == TAttribute::id,
                typename AttributeTypeDescriptor::Type,
                typename ParentType::AttributeTypeById<attributeId>::Type>;
        };

        template<auto attributeId>
        struct Attribute
        {
            using TrivialMemberType = typename AttributeTrivialMemberType<attributeId>::Type;
            using Type = typename AttributeTypeById<attributeId>::Type;
            static const std::size_t size = AttributeSize<attributeId>::value;
            static const std::size_t trivialMemberCount = AttributeTrivialMemberCount<attributeId>::value;
            static const std::size_t offset = AttributeOffset<attributeId>::value;
        };
    };
}