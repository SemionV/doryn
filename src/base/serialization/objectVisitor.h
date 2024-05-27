#pragma once

#include "reflection.h"
#include "base/typeComponents.h"

namespace dory::typeMap
{
    struct DefaultValuePolicy
    {
        template<typename T, typename TContext>
        inline static void process(T&& value, TContext& context)
        {
        }
    };

    struct DefaultOptionalValuePolicy
    {
        template<typename T, typename TContext>
        inline static bool exists(T&& value, TContext& context)
        {
            return false;
        }
    };

    struct DefaultObjectPolicy
    {
        template<typename TContext>
        inline static void beginObject(TContext& context)
        {
        }

        template<typename TContext>
        inline static void endObject(TContext& context)
        {
        }
    };

    struct DefaultMemberPolicy
    {
        template<typename TContext>
        inline static void beginMember(const std::string_view& memberName, const std::size_t i, TContext& context)
        {
        }

        template<typename TContext>
        inline static void endMember(const bool lastMember, TContext& context)
        {
        }
    };

    struct DefaultCollectionPolicy
    {
        template<typename T, auto N, typename TContext>
        inline static void beginCollection(TContext& context)
        {
        }

        template<typename TContext>
        inline static void endCollection(TContext& context)
        {
        }
    };

    struct DefaultCollectionItemPolicy
    {
        template<typename TContext>
        inline static void beginItem(const std::size_t i, TContext& context)
        {
        }

        template<typename TContext>
        inline static void endItem(const bool lastItem, TContext& context)
        {
        }
    };

    struct DefaultDynamicCollectionPolicy
    {
        template<typename T, typename TContext>
        inline static void beginCollection(T& collection, TContext& context)
        {
        }

        template<typename T, typename TContext>
        inline static void endCollection(T& collection, TContext& context)
        {
        }

        template<typename T, typename TContext>
        inline static std::optional<std::reference_wrapper<typename T::value_type>> nextItem(T& collection, TContext& context)
        {
            return {};
        }

        template<typename T, typename V, typename TContext>
        inline static void endItem(std::reference_wrapper<typename T::value_type> item, V& collection, TContext& context)
        {
        }
    };

    struct VisitorDefaultPolicies
    {
        using ValuePolicy = DefaultValuePolicy;
        using OptionalValuePolicy = DefaultOptionalValuePolicy;
        using ObjectPolicy = DefaultObjectPolicy;
        using MemberPolicy = DefaultMemberPolicy;
        using CollectionPolicy = DefaultCollectionPolicy;
        using CollectionItemPolicy = DefaultCollectionItemPolicy;
        using DynamicCollectionPolicyType = DefaultDynamicCollectionPolicy;
    };

    template<typename TPolicies = VisitorDefaultPolicies>
    class ObjectVisitor
    {
    public:
        template<typename T, typename TContext>
        requires(std::is_fundamental_v<std::remove_reference_t<T>>)
        static void visit(T&& object, TContext& context)
        {
            TPolicies::ValuePolicy::process(std::forward<T>(object), context);
        }

        template<typename T, typename TContext>
        requires(std::is_same_v<std::decay_t<T>, std::string>)
        static void visit(T&& object, TContext& context)
        {
            TPolicies::ValuePolicy::process(std::forward<T>(object), context);
        }

        template<typename T, typename TContext>
        requires(is_optional_v<std::decay_t<T>>)
        static void visit(T&& object, TContext& context)
        {
            if(TPolicies::OptionalValuePolicy::exists(std::forward<T>(object), context))
            {
                visit(*object, context);
            }
        }

        template<typename T, typename TContext>
        requires(is_fixed_array_v<std::decay_t<T>>)
        static void visit(T&& object, TContext& context)
        {
            using TArray = std::decay_t<T>;
            constexpr const typename TArray::size_type size = array_size<TArray>::size;

            TPolicies::CollectionPolicy::template beginCollection<typename TArray::value_type, size>(context);

            for(std::size_t i {}; i < size; ++i)
            {
                TPolicies::CollectionItemPolicy::beginItem(i, context);
                visit(object[i], context);
                TPolicies::CollectionItemPolicy::endItem(i == size - 1, context);
            }

            TPolicies::CollectionPolicy::endCollection(context);
        }

        template<typename T, typename TContext>
        requires(is_dynamic_collection_v<std::decay_t<T>>)
        static void visit(T&& object, TContext& context)
        {
            TPolicies::DynamicCollectionPolicyType::beginCollection(std::forward<T>(object), context);

            auto item = TPolicies::DynamicCollectionPolicyType::nextItem(std::forward<T>(object), context);
            while(item)
            {
                auto valueRef = *item;

                visit(valueRef.get(), context);
                TPolicies::DynamicCollectionPolicyType::endItem(valueRef, std::forward<T>(object), context);
                item = TPolicies::DynamicCollectionPolicyType::nextItem(std::forward<T>(object), context);
            }

            TPolicies::DynamicCollectionPolicyType::endCollection(std::forward<T>(object), context);
        }

        template<typename T, typename TContext>
        requires(std::is_class_v<std::decay_t<T>>
                 && !is_fixed_array_v<std::decay_t<T>>
                 && !std::is_same_v<std::decay_t<T>, std::string>
                 && !is_optional_v<std::decay_t<T>>
                 && !is_dynamic_collection_v<std::decay_t<T>>)
        static void visit(T&& object, TContext& context)
        {
            TPolicies::ObjectPolicy::beginObject(context);

            reflection::visitClassFields(object, [](auto& memberValue, const std::string_view& memberName,
                                                    const std::size_t i, const std::size_t memberCount, TContext& context)
            {
                TPolicies::MemberPolicy::beginMember(memberName, i, context);
                visit(memberValue, context);
                TPolicies::MemberPolicy::endMember(i == memberCount - 1, context);
            }, context);

            TPolicies::ObjectPolicy::endObject(context);
        }
    };
}
