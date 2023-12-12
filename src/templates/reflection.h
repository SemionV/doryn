#pragma once

namespace dory::reflection
{
    template <std::size_t N>
    constexpr decltype(auto) makeConstString(const char(&str)[N]) noexcept
    {
        return refl::util::make_const_string(str);
    }

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

    template<typename T, unsigned Idx>
    using MemberValueTypeT = typename MemberValueType<T, Idx>::Type;

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

    template<typename T, int Idx>
    struct TrivialMemberFieldsType;

    template<typename T, bool isRoot, bool isTrivialTypeMember = std::is_trivial_v<T>>
    struct MemberFieldTrivialType
    {
        using Type = T;
    };

    template<typename T, bool isTrivialTypeMember>
    struct MemberFieldTrivialType<T, true, isTrivialTypeMember>
    {
        using Type = T;
    };

    template<typename T>
    struct MemberFieldTrivialType<T, true, false>
    {
        using Type = typename TrivialMemberFieldsType<T, MemberCountV<T> - 1>::Type;
    };

    template<typename T, auto N>
    struct MemberFieldTrivialType<std::array<T, N>, true, false>
    {
        using Type = typename MemberFieldTrivialType<T, false>::Type;
    };

    template<typename T>
    struct MemberFieldTrivialType<T, false, false>: public MemberFieldTrivialType<T, true, false>
    {};

    template<typename T, int Idx>
    struct TrivialMemberFieldsType
    {
        private:
            using MemberValueType = MemberValueTypeT<T, Idx>;
            using NestedMemberTrivialType = MemberFieldTrivialType<MemberValueType, false>;
            using ChoosenType = std::conditional_t<std::is_trivial_v<NestedMemberTrivialType>,
                    NestedMemberTrivialType, TrivialMemberFieldsType<T, Idx - 1>>;

        public:
            using Type = ChoosenType::Type;
    };

    template<typename T>
    struct TrivialMemberFieldsType<T, -1>
    {
        using Type = void;
    };

    template<typename T, int Idx>
    struct TypeMembersSize;

    template<typename T, bool isTrivialTypeMember = std::is_trivial_v<T>>
    struct TypeSize;

    template<typename T>
    static constexpr std::size_t TypeSizeV = TypeSize<T>::value;

    template<typename T, bool isTrivialTypeMember>
    struct TypeSize
    {
        static constexpr std::size_t value = sizeof(T);
    };

    template<typename T>
    struct TypeSize<T, false>
    {
        static constexpr std::size_t value = TypeMembersSize<T, MemberCountV<T> - 1>::value;
    };

    template<typename T, auto N>
    struct TypeSize<std::array<T, N>, false>
    {
        static constexpr std::size_t value = TypeSizeV<T>* N;
    };

    template<typename T, int Idx>
    static constexpr std::size_t TypeMembersSizeV = TypeMembersSize<T, Idx>::value;

    template<typename T, int Idx>
    struct TypeMembersSize
    {
        static constexpr std::size_t value = TypeSizeV<MemberValueTypeT<T, Idx>> + TypeMembersSizeV<T, Idx - 1>;
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
    struct TypeCount;

    template<typename T>
    struct TypeCount<T, true, true>
    {
        static constexpr std::size_t value = 1;
    };

    template<typename T, auto N>
    struct TypeCount<std::array<T, N>, true, true>
    {
        static constexpr std::size_t value = N;
    };

    template<typename T>
    struct TypeCount<T, false, true>: public TypeCount<T, true, true>
    {
    };

    template<typename T>
    struct TypeCount<T, true, false>
    {
        static constexpr std::size_t value = TypeTrivialMembersCount<T, MemberCountV<T> - 1>::value;
    };

    template<typename T, auto N>
    struct TypeCount<std::array<T, N>, true, false>
    {
        static constexpr std::size_t value = TypeCount<T, false>::value * N;
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