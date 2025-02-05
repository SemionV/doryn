#pragma once

namespace dory::serialization::object
{
    template<typename TNode, typename TContextPolicies>
    struct ObjectCopyContext: generic::serialization::TreeStructureContext<const TNode*, TContextPolicies>
    {
        using NodeType = const TNode*;

        explicit ObjectCopyContext(const TNode* root, generic::serialization::Context<TContextPolicies>& otherContext):
            generic::serialization::TreeStructureContext<const TNode*, TContextPolicies>(root, otherContext)
        {}

        explicit ObjectCopyContext(const TNode* root, generic::serialization::Context<TContextPolicies>&& otherContext):
            generic::serialization::TreeStructureContext<const TNode*, TContextPolicies>(root, otherContext)
        {}
    };

    struct ObjectCopyValuePolicy
    {
        template<typename T, typename U, typename TContextPolicies>
        static void process(T&& value, ObjectCopyContext<U, TContextPolicies>& context)
        {
            if(context.node)
            {
                value = *context.node;
            }
        }
    };

    struct ObjectCopyMemberPolicy
    {
        template<typename T, typename TValue, typename TContextPolicies>
        static std::optional<ObjectCopyContext<TValue, TContextPolicies>> beginMember(reflection::ClassMember<T, TValue>& member, const std::size_t i, ObjectCopyContext<T, TContextPolicies>& context)
        {
            if(context.node)
            {
                auto& memberValue = *context.node.*member.pointer;
                return ObjectCopyContext{ &memberValue, context };
            }

            return {};
        }

        template<typename T, typename TValue, typename TContextPolicies>
        static std::optional<ObjectCopyContext<TValue, TContextPolicies>> beginMember(reflection::ClassMember<T, std::optional<TValue>>& member,
            const std::size_t i, ObjectCopyContext<T, TContextPolicies>& context)
        {
            if(context.node)
            {
                if(auto& memberValue = *context.node.*member.pointer)
                {
                    if(!member.value.has_value())
                    {
                        member.value.emplace();
                    }

                    return ObjectCopyContext{ &memberValue.value(), context };
                }
            }

            return {};
        }

        template<typename U, typename TContextPolicies>
        static void endMember(const bool lastMember, ObjectCopyContext<U, TContextPolicies>& context)
        {}
    };

    struct ObjectCopyCollectionPolicy
    {
        template<typename T, auto N, typename TCollection, typename TContextPolicies>
        static void beginCollection(TCollection&& collection, ObjectCopyContext<std::remove_reference_t<TCollection>, TContextPolicies>& context)
        {
			if(context.node)
            {
                collection = *context.node;
            }
        }

        template<typename TCollection, typename TContextPolicies>
        static void endCollection(ObjectCopyContext<TCollection, TContextPolicies>& context)
        {}
    };

    struct ObjectCopyCollectionItemPolicy
    {
        template<typename TCollection, typename TContextPolicies>
        static std::optional<ObjectCopyContext<typename TCollection::value_type, TContextPolicies>> beginItem(const std::size_t i, ObjectCopyContext<TCollection, TContextPolicies>& context)
        {
            //collection is copied already as a whole, no need to copy the items individually
            return {};
        }

        template<typename TItem, typename TContextPolicies>
        static void endItem(const bool lastItem, ObjectCopyContext<TItem, TContextPolicies>& context)
        {
        }
    };

    struct ObjectCopyContainerPolicy: ContainerPolicy<ObjectCopyContainerPolicy>
    {
        template<typename TDestination, typename TSource>
        static void setCollectionSize(TDestination& destination, TSource* source, std::size_t& size)
        {
            size = source ? source->size() : 0;
        }

        template<typename TDestination, typename TSource, typename TDestinationItem, typename TContextPolicies>
        requires(generic::is_dynamic_collection_v<TDestination>)
        static std::optional<ObjectCopyContext<typename TSource::value_type, TContextPolicies>> getCollectionItem(ObjectCopyContext<TSource, TContextPolicies>& context,
            TDestination& destination, auto& index, const TSource* source, TDestinationItem** item)
        {
            assert(source);
            *item = &destination.emplace_back(TDestinationItem{});
            return ObjectCopyContext{ &source->operator[](index), context };
        }

        template<typename TDestination, typename TSource, typename TDestinationItem, typename TContextPolicies>
        requires(generic::is_dictionary_v<TDestination>)
        static std::optional<ObjectCopyContext<typename TSource::mapped_type, TContextPolicies>> getCollectionItem(ObjectCopyContext<TSource, TContextPolicies>& context,
            TDestination& destination, auto& index, const TSource* source, TDestinationItem** item)
        {
            assert(source);

            auto& pair = getMapItem(*source, index);

            auto it = destination.find(pair.first);
            if(it != destination.end())
            {
                *item = &it->second;
            }
            else if(auto result = destination.emplace(pair.first, TDestinationItem{}); result.second)
            {
                *item = &result.first->second;
            }
            else
            {
                //in case if normal emplace to the dictionary did not work, we return an item for default value of the key
                *item = &destination.emplace(typename TDestination::key_type{}, TDestinationItem{}).first->second;
            }

            return ObjectCopyContext{ &pair.second, context };
        }
    };

    struct ObjectCopyPolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = ObjectCopyValuePolicy;
        using EnumPolicy = ObjectCopyValuePolicy;
        using MemberPolicy = ObjectCopyMemberPolicy;
        using CollectionPolicy = ObjectCopyCollectionPolicy;
        using CollectionItemPolicy = ObjectCopyCollectionItemPolicy;
        using ContainerPolicyType = ObjectCopyContainerPolicy;
    };

    template<typename T, typename TContextPolicies, typename... TBaseVisitors>
    static void copy(const T& source, T& target, generic::serialization::Context<TContextPolicies>&& contextBase)
    {
        ObjectCopyContext<T, TContextPolicies> context(&source, contextBase);
        ObjectVisitor<ObjectCopyPolicies, TBaseVisitors...>::visit(target, context);
    }
}