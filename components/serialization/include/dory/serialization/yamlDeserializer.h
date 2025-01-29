#pragma once

#include "objectVisitor.h"
#include "yamlSerializationContext.h"

namespace dory::serialization::yaml
{
    struct DeserializerValuePolicy
    {
    private:
        template<typename T>
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
        template<typename T>
        static void process(T& value, YamlContext& context)
        {
            if(context.node.has_val())
            {
                readValue(value, context.node);
            }
        }
    };

    struct DeserializerEnumPolicy
    {
        template<typename T>
        static void process(T& value, YamlContext& context)
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
        static std::optional<YamlContext> beginMemberGeneric(auto&& member, const std::size_t i, YamlContext& context)
        {
            auto current = context.node;
            const auto& name = toRymlCStr(member.name);
            if(current.is_map() && current.has_child(name))
            {
                return YamlContext{ current[name] };
            }

            return {};
        }

        static std::optional<YamlContext> beginMember(auto&& member, const std::size_t i, YamlContext& context)
        {
            return beginMemberGeneric(member, i, context);
        }

        template<class T, class TValue>
        static std::optional<YamlContext> beginMember(reflection::ClassMember<T, std::optional<TValue>>& member, const std::size_t i, YamlContext& context)
        {
            std::optional<TValue> tempValue;
            reflection::ClassMember<T, std::optional<TValue>> tempMember {
                member.name,
                member.pointer,
                tempValue
            };

            if(auto memberContextOptional = beginMemberGeneric(tempMember, i, context))
            {
                member.value.emplace();
                return memberContextOptional;
            }

            return {};
        }

        static void endMember(const bool lastMember, YamlContext& context)
        {}
    };

    struct DeserializerCollectionItemPolicy
    {
        static std::optional<YamlContext> beginItem(const std::size_t i, YamlContext& context)
        {
            auto current = context.node;
            if(current.is_seq() && i < current.num_children())
            {
                return YamlContext{ current.at(i) };
            }

            return {};
        }

        static void endItem(const bool lastItem, YamlContext& context)
        {}
    };

    struct DeserializerContainerPolicy: public ContainerPolicy<DeserializerContainerPolicy, TreeStructureContext<ryml::NodeRef>>
    {
        using ContextType = TreeStructureContext<ryml::NodeRef>;

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

        template<typename TCollection>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static auto& getCollectionItem(TCollection& collection, auto& index, NodeType& collectionNode, ContextType& itemContext)
        {
            auto itemNode = collectionNode.at(index);
            itemContext = ContextType{ itemNode };

            return collection.emplace_back(typename TCollection::value_type{});
        }

        template<typename TCollection>
        requires(generic::is_dictionary_v<TCollection>)
        static auto& getCollectionItem(TCollection& collection, auto& index, NodeType& collectionNode, ContextType& itemContext)
        {
            using ValueType = typename TCollection::mapped_type;

            if(collectionNode.is_map())
            {
                auto itemNode = collectionNode.at(index);
                itemContext = ContextType{ itemNode };
                auto nodeKey = itemNode.key();
                const auto keyString = std::string_view(nodeKey.str, nodeKey.len);
                const auto key = getKeyValue<TCollection>(keyString);

                auto it = collection.find(key);
                if(it != collection.end())
                {
                    return it->second;
                }

                if(auto result = collection.emplace(key, ValueType{}); result.second)
                {
                    return result.first->second;
                }
            }

            return collection[typename TCollection::key_type{}]; //in case if normal emplace to the dictionary did not work, we return an item for default value of the key
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

    template<typename T, typename... TVisitorBases>
    static void deserialize(std::string source, T& object)
    {
        auto tree = ryml::parse_in_place(toRymlStr(source));
        YamlContext context(tree.rootref());
        ObjectVisitor<YamlDeserializationPolicies, TVisitorBases...>::visit(object, context);
    }

    template<typename T, typename... TVisitorBases>
    static T deserialize(std::string source)
    {
        auto object = T{};

        deserialize<T, TVisitorBases...>(source, object);

        return object;
    }
}