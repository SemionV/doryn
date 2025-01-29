#pragma once

namespace dory::serialization::object
{
    template<typename TNode>
    struct ObjectCopyContext: TreeStructureContext<const TNode*>
    {
        using NodeType = const TNode*;

        ObjectCopyContext() = default;

        explicit ObjectCopyContext(const TNode* root): TreeStructureContext<NodeType>(root)
        {}
    };

    struct ObjectCopyValuePolicy
    {
        template<typename T, typename U>
        static void process(T&& value, ObjectCopyContext<U>& context)
        {
            if(context.node)
            {
                value = *context.node;
            }
        }
    };

    struct ObjectCopyMemberPolicy
    {
        template<typename T, typename TValue>
        static std::optional<ObjectCopyContext<TValue>> beginMember(reflection::ClassMember<T, TValue>& member, const std::size_t i, ObjectCopyContext<T>& context)
        {
            if(context.node)
            {
                auto& memberValue = *context.node.*member.pointer;
                return ObjectCopyContext{ &memberValue };
            }

            return {};
        }

        template<typename T, typename TValue>
        static std::optional<ObjectCopyContext<TValue>> beginMember(reflection::ClassMember<T, std::optional<TValue>>& member, const std::size_t i, ObjectCopyContext<T>& context)
        {
            if(context.node)
            {
                if(auto& memberValue = *context.node.*member.pointer)
                {
                    if(!member.value.has_value())
                    {
                        member.value.emplace();
                    }

                    return ObjectCopyContext{ &memberValue.value() };
                }
            }

            return {};
        }

        template<typename U>
        static void endMember(const bool lastMember, ObjectCopyContext<U>& context)
        {}
    };

    struct ObjectCopyCollectionPolicy
    {
        template<typename T, auto N, typename TCollection>
        static void beginCollection(TCollection&& collection, ObjectCopyContext<std::remove_reference_t<TCollection>>& context)
        {
			if(context.node)
            {
                collection = *context.node;
            }
        }

        template<typename TCollection>
        static void endCollection(ObjectCopyContext<TCollection>& context)
        {}
    };

    struct ObjectCopyCollectionItemPolicy
    {
        template<typename TCollection>
        static std::optional<ObjectCopyContext<typename TCollection::value_type>> beginItem(const std::size_t i, ObjectCopyContext<TCollection>& context)
        {
            //collection is copied already as a whole, no need to copy the items individually
            return {};
        }

        template<typename TItem>
        static void endItem(const bool lastItem, ObjectCopyContext<TItem>& context)
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

        template<typename TDestination, typename TSource, typename TDestinationItem>
        requires(generic::is_dynamic_collection_v<TDestination>)
        static std::optional<ObjectCopyContext<typename TSource::value_type>> getCollectionItem(TDestination& destination, auto& index, TSource* source, TDestinationItem** item)
        {
            assert(source);
            *item = &destination.emplace_back(TDestinationItem{});
            return ObjectCopyContext{ &source->operator[](index) };
        }

        template<typename TDestination, typename TSource, typename TDestinationItem>
        requires(generic::is_dictionary_v<TDestination>)
        static std::optional<ObjectCopyContext<typename TSource::mapped_type>> getCollectionItem(TDestination& destination, auto& index, TSource* source, TDestinationItem** item)
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

            return ObjectCopyContext{ &pair.second };
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

    template<typename T, typename... TBaseVisitors>
    static void copy(const T& source, T& target)
    {
        ObjectCopyContext context(&source);
        ObjectVisitor<ObjectCopyPolicies, TBaseVisitors...>::visit(target, context);
    }
}