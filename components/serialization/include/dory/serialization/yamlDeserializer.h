#pragma once

#include "objectVisitor.h"
#include "yamlSerializationContext.h"

namespace dory::serialization::yaml
{
    struct DeserializerValuePolicy
    {
    private:
        template<typename T>
        requires (!std::is_same_v<T, std::string>)
        static void readValue(T& value, ryml::NodeRef& node)
        {
            node >> value;
        }

        static void readValue(std::string& value, const ryml::NodeRef& node)
        {
            const auto yamlValue = node.val();
            value = std::string(yamlValue.str, yamlValue.len);
        }

    public:
        template<typename T, typename TContextPolicies>
        static void process(T& value, YamlContext<TContextPolicies>& context)
        {
            if(context.node.has_val())
            {
                readValue(value, context.node);
            }
        }
    };

    struct DeserializerEnumPolicy
    {
        template<typename T, typename TContextPolicies>
        static void process(T& value, YamlContext<TContextPolicies>& context)
        {
            if(const auto current = context.node; current.has_val())
            {
                const auto yamlValue = current.val();
                const auto enumName = std::string_view(yamlValue.str, yamlValue.len);

                if(auto enumValue = magic_enum::enum_cast<T>(enumName))
                {
                    value = *enumValue;
                }
            }
        }
    };

    struct DeserializerMemberPolicy
    {
        template<typename TContextPolicies>
        static std::optional<YamlContext<TContextPolicies>> beginMemberGeneric(auto&& member, const std::size_t i, YamlContext<TContextPolicies>& context)
        {
            auto current = context.node;
            const auto& name = toRymlCStr(member.name);
            if(current.is_map() && current.has_child(name))
            {
                return YamlContext{ current[name], context };
            }

            return {};
        }

        template<typename TContextPolicies>
        static std::optional<YamlContext<TContextPolicies>> beginMember(auto&& member, const std::size_t i, YamlContext<TContextPolicies>& context)
        {
            return beginMemberGeneric(member, i, context);
        }

        template<class T, class TValue, typename TContextPolicies>
        static std::optional<YamlContext<TContextPolicies>> beginMember(reflection::ClassMember<T, std::optional<TValue>>& member, const std::size_t i, YamlContext<TContextPolicies>& context)
        {
            if(auto memberContext = beginMemberGeneric(member, i, context))
            {
                member.value.emplace();
                return memberContext;
            }

            return {};
        }

        template<typename TContextPolicies>
        static void endMember(const bool lastMember, YamlContext<TContextPolicies>& context)
        {}
    };

    struct DeserializerCollectionItemPolicy
    {
        template<typename TContextPolicies>
        static std::optional<YamlContext<TContextPolicies>> beginItem(const std::size_t i, const YamlContext<TContextPolicies>& context)
        {
            auto current = context.node;
            if(current.is_seq() && i < current.num_children())
            {
                return YamlContext{ current.at(i), context };
            }

            return {};
        }

        template<typename TContextPolicies>
        static void endItem(const bool lastItem, YamlContext<TContextPolicies>& context)
        {}
    };

    struct DeserializerContainerPolicy: public ContainerPolicy<DeserializerContainerPolicy>
    {
        using NodeType = ryml::NodeRef;

        template<typename TCollection>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static void setCollectionSize(TCollection& collection, NodeType& collectionNode, std::size_t& size)
        {
            size = collectionNode.is_seq() ? collectionNode.num_children() : 0;
        }

        template<typename TCollection>
        requires(generic::is_dictionary_v<TCollection>)
        static void setCollectionSize(TCollection& collection, NodeType& collectionNode, std::size_t& size)
        {
            if(collectionNode.is_map())
            {
                size = collectionNode.num_children();
            }
            else
            {
                size = 0;
            }
        }

        template<typename TCollection, typename TItem, typename TContextPolicies>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static std::optional<YamlContext<TContextPolicies>> getCollectionItem(YamlContext<TContextPolicies>& context,
            TCollection& collection, auto& index, NodeType& collectionNode, TItem** item)
        {
            auto itemNode = collectionNode.at(index);
            *item = &collection.emplace_back(typename TCollection::value_type{});
            return YamlContext{ itemNode, context };
        }

        template<typename TCollection, typename TItem, typename TContextPolicies>
        requires(generic::is_dictionary_v<TCollection>)
        static std::optional<YamlContext<TContextPolicies>> getCollectionItem(YamlContext<TContextPolicies>& context,
            TCollection& collection, auto& index, NodeType& collectionNode, TItem** item)
        {
            if(collectionNode.is_map())
            {
                auto itemNode = collectionNode.at(index);
                auto nodeKey = itemNode.key();
                const auto keyString = std::string_view(nodeKey.str, nodeKey.len);
                const auto key = getKeyValue<TCollection>(keyString);

                auto it = collection.find(key);
                if(it != collection.end())
                {
                    *item = &it->second;
                }
                else if(auto result = collection.emplace(key, TItem{}); result.second)
                {
                    *item = &result.first->second;
                }
                else
                {
                    //in case if normal emplace to the dictionary did not work, try to emplace with default key value
                    *item = &collection.emplace(typename TCollection::key_type{}, TItem{}).first->second;
                }

                return YamlContext{ itemNode, context };
            }

            return {};
        }
    };

    struct YamlDeserializationPolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = DeserializerValuePolicy;
        using EnumPolicy = DeserializerEnumPolicy;
        using MemberPolicy = DeserializerMemberPolicy;
        using CollectionItemPolicy = DeserializerCollectionItemPolicy;
        using ContainerPolicyType = DeserializerContainerPolicy;
    };

    template<typename T, typename TContextPolicies, typename... TVisitorBases>
    static void deserialize(T& object, YamlContext<TContextPolicies>& context)
    {
        ObjectVisitor<YamlDeserializationPolicies, TVisitorBases...>::visit(object, context);
    }

    template<typename T, typename TContextPolicies, typename... TVisitorBases>
    static void deserialize(const std::string& source, T& object, generic::serialization::Context<TContextPolicies>&& contextBase)
    {
        auto tree = ryml::parse_in_arena(toRymlCStr(source));
        YamlContext<TContextPolicies> context(tree.rootref(), contextBase);
        deserialize<T, TContextPolicies, TVisitorBases...>(object, context);
    }

    template<typename T, typename TContextPolicies, typename... TVisitorBases>
    static T deserialize(const std::string& source, generic::serialization::Context<TContextPolicies>&& contextBase)
    {
        auto object = T{};
        deserialize<T, TContextPolicies, TVisitorBases...>(std::move(source), object, std::move(contextBase));
        return object;
    }
}