#pragma once

namespace dory::serialization::object
{
    struct ObjectCopyContext: TreeStructureContext<const void*>
    {
        explicit ObjectCopyContext(const void* root): TreeStructureContext(root)
        {}
    };

    struct ObjectCopyValuePolicy
    {
        template<typename T>
        static void process(T&& value, ObjectCopyContext& context)
        {
            if(const auto current = context.parents.top())
            {
                value = *static_cast<const std::decay_t<T>*>(current);
            }
        }
    };

    struct ObjectCopyEnumPolicy
    {
        template<typename T>
        static void process(T&& value, ObjectCopyContext& context)
        {
            if(const auto current = context.parents.top())
            {
                value = *static_cast<const std::decay_t<T>*>(current);
            }
        }
    };

    struct ObjectCopyMemberPolicy
    {
        template<class T, class TValue>
        static bool beginMember(reflection::ClassMember<T, TValue>& member, const std::size_t i, ObjectCopyContext& context)
        {
            if(const auto currentContext = context.parents.top())
            {
                auto& currentValue = (*static_cast<const std::decay_t<T>*>(currentContext)).*member.pointer;
                context.parents.push(&currentValue);

                return true;
            }

            return false;
        }

        template<class T, class TValue>
        requires(generic::is_optional_v<std::decay_t<TValue>>)
        static bool beginMember(reflection::ClassMember<T, TValue>& member, const std::size_t i, ObjectCopyContext& context)
        {
            if(const auto currentContext = context.parents.top())
            {
                if(auto& currentValue = *static_cast<const std::decay_t<T>*>(currentContext).*member.pointer)
                {
                    context.parents.push(&currentValue.value());
                    if(!member.value.has_value())
                    {
                        member.value.emplace();
                    }

                    return true;
                }
            }

            return false;
        }

        static void endMember(const bool lastMember, ObjectCopyContext& context)
        {
            context.parents.pop();
        }
    };

    struct ObjectCopyCollectionPolicy
    {
        template<typename T, auto N, typename TCollection>
        inline static void beginCollection(TCollection&& collection, ObjectCopyContext& context)
        {
			if(const auto current = context.parents.top())
            {
                collection = *(const std::decay_t<TCollection>*)current;
            }
        }

        inline static void endCollection(ObjectCopyContext& context)
        {
        }
    };

    struct ObjectCopyCollectionItemPolicy
    {
        static bool beginItem(const std::size_t i, ObjectCopyContext& context)
        {
            return false;
        }

        static void endItem(const bool lastItem, ObjectCopyContext& context)
        {
        }
    };

    struct ObjectCopyContainerPolicy: ContainerPolicy<ObjectCopyContainerPolicy, TreeStructureContext<const void*>>
    {
        template<typename TCollection>
        static void setCollectionSize(TCollection& collection, std::stack<NodeType>& parents, std::size_t& size)
        {
            auto currentContext = parents.top();
            size = currentContext ? static_cast<const TCollection*>(currentContext)->size() : 0;
        }

        template<typename TCollection>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static auto& getCollectionItem(TCollection& collection, auto& index, std::stack<NodeType>& parents)
        {
            auto currentCollection = parents.top();
            assert(currentCollection);

            parents.push(&(*(TCollection*)currentCollection)[index]);

            return collection.emplace_back(typename TCollection::value_type{});
        }

        template<typename TCollection>
        requires(generic::is_dictionary_v<TCollection>)
        static auto& getCollectionItem(TCollection& collection, auto& index, std::stack<NodeType>& parents)
        {
            using ValueType = typename TCollection::mapped_type;

            auto currentCollection = parents.top();
            assert(currentCollection);

            auto& pair = getMapItem(*((const std::decay_t<TCollection>*)currentCollection), index);
            parents.push(&pair.second);

            auto it = collection.find(pair.first);
            if(it != collection.end())
            {
                return it->second;
            }

            if(auto result = collection.emplace(pair.first, ValueType{}); result.second)
            {
                return result.first->second;
            }

            return collection[typename TCollection::key_type{}]; //in case if normal emplace to the dictionary did not work, we return an item for default value of the key
        }
    };

    struct ObjectCopyPolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = ObjectCopyValuePolicy;
        using EnumPolicy = ObjectCopyEnumPolicy;
        using MemberPolicy = ObjectCopyMemberPolicy;
        using CollectionPolicy = ObjectCopyCollectionPolicy;
        using CollectionItemPolicy = ObjectCopyCollectionItemPolicy;
        using ContainerPolicyType = ObjectCopyContainerPolicy;
    };

    template<typename T, typename TBaseVisitor = ObjectVisitorDefaultBase>
    static void copy(const T& source, T& target)
    {
        ObjectCopyContext context(&source);
        ObjectVisitor<ObjectCopyPolicies, TBaseVisitor>::visit(target, context);
    }

    struct ObjectMergeMemberPolicy
    {
        template<class T, class TValue>
        static bool beginMember(reflection::ClassMember<T, TValue>& destinationMember, const std::size_t i, ObjectCopyContext& context)
        {
            if(const auto currentContext = context.parents.top())
            {
                auto& object = *static_cast<const std::decay_t<T>*>(currentContext);
                bool result {};

                reflection::visitClassFields(object, [&destinationMember, &result]<typename TClassMember>(TClassMember&& sourceMember, const std::size_t i, const std::size_t memberCount, ObjectCopyContext& context)
                {
                    if(sourceMember.name == destinationMember.name && !result)
                    {
                        context.parents.push(&sourceMember.value);
                        result = true;
                    }
                }, context);

                return result;
            }

            return false;
        }

        template<class T, class TValue>
        requires(generic::is_optional_v<std::decay_t<TValue>>)
        static bool beginMember(reflection::ClassMember<T, TValue>& destinationMember, const std::size_t i, ObjectCopyContext& context)
        {
            if(const auto currentContext = context.parents.top())
            {
                auto& object = *static_cast<const std::decay_t<T>*>(currentContext);
                bool result {};

                reflection::visitClassFields(object, [&destinationMember, &result]<typename TClassMember>(TClassMember&& sourceMember, const std::size_t i, const std::size_t memberCount, ObjectCopyContext& context)
                {
                    if(sourceMember.name == destinationMember.name && !result)
                    {
                        context.parents.push(&sourceMember.value.value());

                        if(!destinationMember.value.has_value())
                        {
                            destinationMember.value.emplace();
                        }

                        result = true;
                    }
                }, context);

                return result;
            }

            return false;
        }

        static void endMember(const bool lastMember, ObjectCopyContext& context)
        {
            context.parents.pop();
        }
    };

    struct ObjectMergePolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = ObjectCopyValuePolicy;
        using EnumPolicy = ObjectCopyEnumPolicy;
        using MemberPolicy = ObjectMergeMemberPolicy;
        using CollectionPolicy = ObjectCopyCollectionPolicy;
        using CollectionItemPolicy = ObjectCopyCollectionItemPolicy;
        using ContainerPolicyType = ObjectCopyContainerPolicy;
    };

    template<typename T, typename TBaseVisitor = ObjectVisitorDefaultBase>
    static void merge(const T& source, T& target)
    {
        ObjectCopyContext context(&source);
        ObjectVisitor<ObjectMergePolicies, TBaseVisitor>::visit(target, context);
    }
}