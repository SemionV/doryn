#pragma once

#include "reflection.h"

namespace dory::typeMap
{
    struct DefaultValuePolicy
    {
        template<typename T, typename TContext>
        inline static void process(T&& value, TContext& context)
        {
        }
    };

    struct DefaultBeginMemberPolicy
    {
        template<typename TContext>
        inline static void process(const std::string& memberName, const std::size_t i, TContext& context)
        {
        }
    };

    struct DefaultEndMemberPolicy
    {
        template<typename TContext>
        inline static void process(const bool lastMember, TContext& context)
        {
        }
    };

    struct DefaultBeginCollectionItemPolicy
    {
        template<typename TContext>
        inline static void process(const std::size_t i, TContext& context)
        {
        }
    };

    struct DefaultEndCollectionItemPolicy
    {
        template<typename TContext>
        inline static void process(const bool lastItem, TContext& context)
        {
        }
    };

    struct DefaultBeginCollectionPolicy
    {
        template<typename TContext>
        inline static void process(TContext& context)
        {
        }
    };

    struct DefaultEndCollectionPolicy
    {
        template<typename TContext>
        inline static void process(TContext& context)
        {
        }
    };

    struct DefaultBeginObjectPolicy
    {
        template<typename TContext>
        inline static void process(TContext& context)
        {
        }
    };

    struct DefaultEndObjectPolicy
    {
        template<typename TContext>
        inline static void process(TContext& context)
        {
        }
    };

    struct VisitorDefaultPolicies
    {
        using ValuePolicy = DefaultValuePolicy;
        using BeginMemberPolicy = DefaultBeginMemberPolicy;
        using EndMemberPolicy = DefaultEndMemberPolicy;
        using BeginCollectionPolicy = DefaultBeginCollectionPolicy;
        using EndCollectionPolicy = DefaultEndCollectionPolicy;
        using BeginCollectionItemPolicy = DefaultBeginCollectionItemPolicy;
        using EndCollectionItemPolicy = DefaultEndCollectionItemPolicy;
        using BeginObjectPolicy = DefaultBeginObjectPolicy;
        using EndObjectPolicy = DefaultEndObjectPolicy;
    };

    template<typename TPolicies = VisitorDefaultPolicies>
    class ObjectVisitor
    {
    private:
        template<typename T>
        static constexpr std::size_t getClassMemberCount()
        {
            std::size_t count {};
            visitClassMembers<T>([&count](auto memberDescriptor) {
                ++count;
            });

            return count;
        }

        template<typename T, typename F>
        static constexpr void visitClassMembers(F functor)
        {
            refl::descriptor::type_descriptor<std::remove_reference_t<T>> typeDescriptor {};
            for_each(typeDescriptor.members, [&](auto memberDescriptor)
            {
                //so far only for fields, possible to add support for member functions
                if constexpr (is_readable(memberDescriptor) && is_field(memberDescriptor))
                {
                    functor(memberDescriptor);
                }
            });
        }

        template<typename T, typename TContext>
        static void visitClassMember(T& memberValue, const std::string& memberName, const std::size_t i, const std::size_t memberCount, TContext& context)
        {
            TPolicies::BeginMemberPolicy::process(memberName, i, context);
            visit(memberValue, context);
            TPolicies::EndMemberPolicy::process(i == memberCount - 1, context);
        }

        template<typename T, auto N, typename TContext>
        static void visitArray(std::array<T, N>& object, TContext& context)
        {
            TPolicies::BeginCollectionPolicy::process(context);

            for(std::size_t i {}; i < N; ++i)
            {
                TPolicies::BeginCollectionItemPolicy::process(i, context);
                visit(object[i], context);
                TPolicies::EndCollectionItemPolicy::process(i == N - 1, context);
            }

            TPolicies::EndCollectionPolicy::process(context);
        }

    public:
        template<typename T, typename TContext>
        requires(std::is_trivial_v<std::remove_reference_t<T>>)
        static void visit(T&& object, TContext& context)
        {
            TPolicies::ValuePolicy::process(std::forward<T>(object), context);
        }

        template<typename T, typename TContext>
        requires(std::is_class_v<std::remove_reference_t<T>>)
        static void visit(T&& object, TContext& context)
        {
            TPolicies::BeginObjectPolicy::process(context);
            const constexpr auto memberCount = getClassMemberCount<T>();
            std::size_t i = {};

            visitClassMembers<T>([&object, &context, &i, memberCount](auto memberDescriptor) {
                if constexpr (is_field(memberDescriptor))
                {
                    using MemberDescriptorType = decltype(memberDescriptor);

                    auto& memberValue = object.*MemberDescriptorType::pointer;
                    const auto& memberName = (std::string) MemberDescriptorType::name;
                    visitClassMember(memberValue, memberName, i, memberCount, context);
                }

                ++i;
            });

            TPolicies::EndObjectPolicy::process(context);
        }

        template<typename T, auto N, typename TContext>
        static void visit(std::array<T, N>& object, TContext& context)
        {
            visitArray(object, context);
        }

        template<typename T, auto N, typename TContext>
        static void visit(std::array<T, N>&& object, TContext& context)
        {
            visitArray(object, context);
        }
    };
}
