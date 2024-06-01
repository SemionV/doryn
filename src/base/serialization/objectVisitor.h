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
        template<typename TContext, typename T>
        inline static bool beginMember(const std::string_view& memberName, T& value, const std::size_t i, TContext& context)
        {
            return true;
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
        inline static bool beginItem(const std::size_t i, TContext& context)
        {
            return true;
        }

        template<typename TContext>
        inline static void endItem(const bool lastItem, TContext& context)
        {
        }
    };

    struct DefaultContainerPolicy
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
        inline static std::optional<std::reference_wrapper<GetCollectionValueType<T>>> nextItem(T& collection, TContext& context)
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
        using ObjectPolicy = DefaultObjectPolicy;
        using MemberPolicy = DefaultMemberPolicy;
        using CollectionPolicy = DefaultCollectionPolicy;
        using CollectionItemPolicy = DefaultCollectionItemPolicy;
        using ContainerPolicyType = DefaultContainerPolicy;
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
            visit(*object, context);
        }

        template<typename T, typename TContext>
        requires(is_pair_v<std::decay_t<T>>)
        static void visit(T&& object, TContext& context)
        {
            visit(*object.second, context);
        }

        template<typename T, typename TContext>
        requires(is_fixed_array_v<std::decay_t<T>>)
        static void visit(T&& object, TContext& context)
        {
            using TArray = std::decay_t<T>;
            constexpr const typename TArray::size_type size = array_size<TArray>::size;

            TPolicies::CollectionPolicy::template beginCollection<typename TArray::value_type, size>(context);

            size_t lastIndex = size - 1;
            for(std::size_t i {}; i < size; ++i)
            {
                if(TPolicies::CollectionItemPolicy::beginItem(i, context))
                {
                    visit(object[i], context);
                    TPolicies::CollectionItemPolicy::endItem(i == lastIndex, context);
                }
            }

            TPolicies::CollectionPolicy::endCollection(context);
        }

        template<typename T, typename TContext>
        requires(is_dynamic_collection_v<T> || is_dictionary_v<T>)
        static void visit(T&& object, TContext& context)
        {
            TPolicies::ContainerPolicyType::beginCollection(std::forward<T>(object), context);

            auto item = TPolicies::ContainerPolicyType::nextItem(std::forward<T>(object), context);
            while(item)
            {
                auto valueRef = *item;

                visit(valueRef.get(), context);
                TPolicies::ContainerPolicyType::endItem(valueRef, std::forward<T>(object), context);
                item = TPolicies::ContainerPolicyType::nextItem(std::forward<T>(object), context);
            }

            TPolicies::ContainerPolicyType::endCollection(std::forward<T>(object), context);
        }

        template<typename T, typename TContext>
        requires(std::is_class_v<std::decay_t<T>>
                 && !is_fixed_array_v<std::decay_t<T>>
                 && !std::is_same_v<std::decay_t<T>, std::string>
                 && !is_optional_v<std::decay_t<T>>
                 && !is_dynamic_collection_v<T>
                 && !is_dictionary_v<T>
                 && !is_pair_v<std::decay_t<T>>)
        static void visit(T&& object, TContext& context)
        {
            TPolicies::ObjectPolicy::beginObject(context);

            reflection::visitClassFields(object, [](auto& memberValue, const std::string_view& memberName,
                                                    const std::size_t i, const std::size_t memberCount, TContext& context)
            {
                if(TPolicies::MemberPolicy::beginMember(memberName, memberValue, i, context))
                {
                    visit(memberValue, context);
                    TPolicies::MemberPolicy::endMember(i == memberCount - 1, context);
                }
            }, context);

            TPolicies::ObjectPolicy::endObject(context);
        }
    };
}
