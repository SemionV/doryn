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

    struct DefaultBeginCollectionPolicy
    {
        template<typename T, auto N, typename TContext>
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

    struct VisitorDefaultPolicies
    {
        using ValuePolicy = DefaultValuePolicy;
        using BeginObjectPolicy = DefaultBeginObjectPolicy;
        using EndObjectPolicy = DefaultEndObjectPolicy;
        using BeginMemberPolicy = DefaultBeginMemberPolicy;
        using EndMemberPolicy = DefaultEndMemberPolicy;
        using BeginCollectionPolicy = DefaultBeginCollectionPolicy;
        using EndCollectionPolicy = DefaultEndCollectionPolicy;
        using BeginCollectionItemPolicy = DefaultBeginCollectionItemPolicy;
        using EndCollectionItemPolicy = DefaultEndCollectionItemPolicy;
    };

    template<typename TPolicies = VisitorDefaultPolicies>
    class ObjectVisitor
    {
    private:
        template<typename T, auto N, typename TContext>
        static void visitArray(std::array<T, N>& object, TContext& context)
        {
            TPolicies::BeginCollectionPolicy::template process<T, N>(context);

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

            reflection::visitClassFields(object, [](auto& memberValue, const std::string& memberName,
                    const std::size_t i, const std::size_t memberCount, TContext& context)
            {
                TPolicies::BeginMemberPolicy::process(memberName, i, context);
                visit(memberValue, context);
                TPolicies::EndMemberPolicy::process(i == memberCount - 1, context);
            }, context);

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
