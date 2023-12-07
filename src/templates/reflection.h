#pragma once

namespace dory::reflection
{
    template<typename T>
    struct MemberCount
    {
        static constexpr int value =  static_cast<int>(refl::detail::type_info<T>::member_count);
    };

    template<typename T>
    constexpr int MemberCountV = MemberCount<T>::value;

    template<typename T, unsigned Idx>
    struct MemberValueType
    {
        using Type = typename refl::detail::member_info<T, Idx>::value_type;
    };

    template<typename T>
    struct FieldCount;

    template<typename T, typename... Ts>
    struct FieldCount<refl::util::type_list<T, Ts...>>
    {
        static constexpr const std::size_t value = (refl::trait::is_field_v<T> ? 1 : 0) + FieldCount<refl::util::type_list<Ts...>>::value;
    };

    template<>
    struct FieldCount<refl::util::type_list<>>
    {
        static constexpr const std::size_t value = 0;
    };

    template<typename T>
    static constexpr const auto ClassFieldCountV = FieldCount<typename refl::descriptor::type_descriptor<std::remove_reference_t<T>>::member_types>::value;

    template<typename T, typename F, typename... Args>
    constexpr void visitClassFields(T&& object, F functor, Args&&... args)
    {
        std::size_t i = {};
        refl::descriptor::type_descriptor<std::remove_reference_t<T>> typeDescriptor {};
        const auto memberCount = reflection::ClassFieldCountV<T>;

        for_each(typeDescriptor.members, [&](auto memberDescriptor)
        {
            if constexpr (is_field(memberDescriptor))
            {
                using MemberDescriptorType = decltype(memberDescriptor);

                auto& memberValue = object.*MemberDescriptorType::pointer;
                const auto& memberName = (std::string) MemberDescriptorType::name;
                functor(memberValue, memberName, i++, memberCount, std::forward<Args>(args)...);
            }
        });
    }

    template<typename T, unsigned Idx>
    using MemberValueTypeT = typename MemberValueType<T, Idx>::Type;

    template<typename T, int Idx>
    struct TrivialMembersType;

    template<typename T, bool isRoot, bool isTrivialTypeMember = std::is_trivial_v<T>>
    struct MemberTrivialType
    {
        using Type = T;
    };

    template<typename T, bool isTrivialTypeMember>
    struct MemberTrivialType<T, true, isTrivialTypeMember>
    {
        using Type = void;
    };

    template<typename T>
    struct MemberTrivialType<T, true, false>
    {
        using Type = typename TrivialMembersType<T, MemberCountV<T> - 1>::Type;
    };

    template<typename T>
    struct MemberTrivialType<T, false, false>: public MemberTrivialType<T, true, false>
    {};

    template<typename T, int Idx>
    struct TrivialMembersType
    {
        private:
            using MemberValueType = MemberValueTypeT<T, Idx>;
            using NestedMemberTrivialType = MemberTrivialType<MemberValueType, false>;
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

    template<typename T, bool isTrivialTypeMember = std::is_trivial_v<T>>
    struct TypeSize
    {
        static constexpr std::size_t value = sizeof(T);
    };

    template<typename T>
    struct TypeSize<T, false>
    {
        static constexpr std::size_t value = TypeMembersSize<T, MemberCountV<T> - 1>::value;
    };

    template<typename T, int Idx>
    struct TypeMembersSize
    {
        static constexpr std::size_t value = TypeSize<MemberValueTypeT<T, Idx>>::value + TypeMembersSize<T, Idx - 1>::value;
    };

    template<typename T>
    struct TypeMembersSize<T, -1>
    {
        public:
            static constexpr std::size_t value = 0;
    };

    template<typename T, int Idx>
    struct TypeTrivialMembersCount;

    template<typename T, bool isRoot, bool isTrivialTypeMember = std::is_trivial_v<T>>
    struct TypeCount
    {
        static constexpr std::size_t value = 1;
    };

    template<typename T, bool isTrivialTypeMember>
    struct TypeCount<T, true, isTrivialTypeMember>
    {
        static constexpr std::size_t value = 0;
    };

    template<typename T>
    struct TypeCount<T, true, false>
    {
        static constexpr std::size_t value = TypeTrivialMembersCount<T, MemberCountV<T> - 1>::value;
    };

    template<typename T>
    struct TypeCount<T, false, false>: public TypeCount<T, true, false>
    {};

    template<typename T, int Idx>
    struct TypeTrivialMembersCount
    {
        static constexpr std::size_t value = TypeCount<MemberValueTypeT<T, Idx>, false>::value + TypeTrivialMembersCount<T, Idx - 1>::value;
    };

    template<typename T>
    struct TypeTrivialMembersCount<T, -1>
    {
        public:
            static constexpr std::size_t value = 0;
    };
}