#pragma once

#include "baseTests/dependencies.h"

namespace dory::serialization
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
            using ChoosenType = std::conditional_t<std::is_trivial_v<NestedMemberTrivialType>,
                    NestedMemberTrivialType, TrivialMembersType<T, Idx - 1>>;

        public:
            using Type = ChoosenType::Type;
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
        static constexpr std::size_t value = sizeof(T);
    };

    template<typename T>
    struct TypeSize<T, false>
    {
        static constexpr std::size_t value = TypeMembersSize<T, static_cast<int>(refl::detail::type_info<T>::member_count) - 1>::value;
    };

    template<typename T, int Idx>
    struct TypeMembersSize
    {
        private:
            using MemberValueType = typename refl::detail::member_info<T, Idx>::value_type;

        public:
            static constexpr std::size_t value = TypeSize<MemberValueType, std::is_trivial_v<MemberValueType>>::value + 
                TypeMembersSize<T, Idx - 1>::value;
    };

    template<typename T>
    struct TypeMembersSize<T, -1>
    {
        public:
            static constexpr std::size_t value = 0;
    };

    template<typename T, int Idx>
    struct TypeTrivialMembersCount;

    template<typename T, bool isTrivialTypeMember, bool isRoot>
    struct TypeCount
    {
        static constexpr std::size_t value = 1;
    };

    template<typename T, bool isTrivialTypeMember>
    struct TypeCount<T, isTrivialTypeMember, true>
    {
        static constexpr std::size_t value = 0;
    };

    template<typename T>
    struct TypeCount<T, false, true>
    {
        static constexpr std::size_t value = TypeTrivialMembersCount<T, static_cast<int>(refl::detail::type_info<T>::member_count) - 1>::value;
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
            static constexpr std::size_t value = TypeCount<MemberValueType, std::is_trivial_v<MemberValueType>, false>::value + 
                TypeTrivialMembersCount<T, Idx - 1>::value;
    };

    template<typename T>
    struct TypeTrivialMembersCount<T, -1>
    {
        public:
            static constexpr std::size_t value = 0;
    };
}