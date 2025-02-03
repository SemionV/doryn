#pragma once

namespace dory::serialization::object
{
    template<typename TNode, typename TRegistry, typename TDataContext>
    struct ObjectCopyContext: TreeStructureContext<const TNode*, TRegistry, TDataContext>
    {
        using NodeType = const TNode*;

        explicit ObjectCopyContext(const TNode* root, TRegistry& registry, TDataContext& dataContext):
            TreeStructureContext<NodeType, TRegistry, TDataContext>(root, registry, dataContext)
        {}
    };

    struct ObjectCopyValuePolicy
    {
        template<typename T, typename U, typename TRegistry, typename TDataContext>
        static void process(T&& value, ObjectCopyContext<U, TRegistry, TDataContext>& context)
        {
            if(context.node)
            {
                value = *context.node;
            }
        }
    };

    struct ObjectCopyMemberPolicy
    {
        template<typename T, typename TValue, typename TRegistry, typename TDataContext>
        static std::optional<ObjectCopyContext<TValue, TRegistry, TDataContext>> beginMember(reflection::ClassMember<T, TValue>& member, const std::size_t i, ObjectCopyContext<T, TDataContext, TRegistry>& context)
        {
            if(context.node)
            {
                auto& memberValue = *context.node.*member.pointer;
                return ObjectCopyContext{ &memberValue, context.registry, context.dataContext };
            }

            return {};
        }

        template<typename T, typename TValue, typename TRegistry, typename TDataContext>
        static std::optional<ObjectCopyContext<TValue, TRegistry, TDataContext>> beginMember(reflection::ClassMember<T, std::optional<TValue>>& member,
            const std::size_t i, ObjectCopyContext<T, TRegistry, TDataContext>& context)
        {
            if(context.node)
            {
                if(auto& memberValue = *context.node.*member.pointer)
                {
                    if(!member.value.has_value())
                    {
                        member.value.emplace();
                    }

                    return ObjectCopyContext{ &memberValue.value(), context.registry, context.dataContext };
                }
            }

            return {};
        }

        template<typename U, typename TRegistry, typename TDataContext>
        static void endMember(const bool lastMember, ObjectCopyContext<U, TRegistry, TDataContext>& context)
        {}
    };

    struct ObjectCopyCollectionPolicy
    {
        template<typename T, auto N, typename TCollection, typename TRegistry, typename TDataContext>
        static void beginCollection(TCollection&& collection, ObjectCopyContext<std::remove_reference_t<TCollection>, TRegistry, TDataContext>& context)
        {
			if(context.node)
            {
                collection = *context.node;
            }
        }

        template<typename TCollection, typename TRegistry, typename TDataContext>
        static void endCollection(ObjectCopyContext<TCollection, TRegistry, TDataContext>& context)
        {}
    };

    struct ObjectCopyCollectionItemPolicy
    {
        template<typename TCollection, typename TRegistry, typename TDataContext>
        static std::optional<ObjectCopyContext<typename TCollection::value_type, TRegistry, TDataContext>> beginItem(const std::size_t i, ObjectCopyContext<TCollection, TRegistry, TDataContext>& context)
        {
            //collection is copied already as a whole, no need to copy the items individually
            return {};
        }

        template<typename TItem, typename TRegistry, typename TDataContext>
        static void endItem(const bool lastItem, ObjectCopyContext<TItem, TRegistry, TDataContext>& context)
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

        template<typename TDestination, typename TSource, typename TDestinationItem, typename TRegistry, typename TDataContext>
        requires(generic::is_dynamic_collection_v<TDestination>)
        static std::optional<ObjectCopyContext<typename TSource::value_type, TRegistry, TDataContext>> getCollectionItem(ObjectCopyContext<TSource, TRegistry, TDataContext>& context,
            TDestination& destination, auto& index, const TSource* source, TDestinationItem** item)
        {
            assert(source);
            *item = &destination.emplace_back(TDestinationItem{});
            return ObjectCopyContext{ &source->operator[](index), context.registry, context.dataContext };
        }

        template<typename TDestination, typename TSource, typename TDestinationItem, typename TRegistry, typename TDataContext>
        requires(generic::is_dictionary_v<TDestination>)
        static std::optional<ObjectCopyContext<typename TSource::mapped_type, TRegistry, TDataContext>> getCollectionItem(ObjectCopyContext<TSource, TRegistry, TDataContext>& context,
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

            return ObjectCopyContext{ &pair.second, context.registry, context.dataContext };
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

    template<typename T, typename TRegistry, typename TDataContext, typename... TBaseVisitors>
    static void copy(const T& source, T& target, TRegistry& registry, TDataContext& dataContext)
    {
        ObjectCopyContext<T, TRegistry, TDataContext> context(&source, registry, dataContext);
        ObjectVisitor<ObjectCopyPolicies, TBaseVisitors...>::visit(target, context);
    }
}