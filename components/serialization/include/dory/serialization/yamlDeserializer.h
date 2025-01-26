#pragma once

#include "objectVisitor.h"
#include "yamlSerializationContext.h"

namespace dory::serialization::yaml
{
    struct DeserializerValuePolicy
    {
    private:
        template<typename T>
        inline static void readValue(T& value, ryml::NodeRef& node)
        {
            node >> value;
        }

        inline static void readValue(std::string& value, ryml::NodeRef& node)
        {
            auto yamlValue = node.val();
            value = std::string(yamlValue.str, yamlValue.len);
        }

    public:
        template<typename T>
        inline static void process(T& value, YamlContext& context)
        {
            auto current = context.parents.top();
            if(current.has_val())
            {
                readValue(value, current);
            }
        }
    };

    struct DeserializerEnumPolicy
    {
        template<typename T>
        static void process(T& value, YamlContext& context)
        {
            if(const auto current = context.parents.top(); current.has_val())
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
        static bool beginMemberGeneric(auto&& member, const std::size_t i, YamlContext& context)
        {
            auto current = context.parents.top();
            const auto& name = toRymlCStr(member.name);
            if(current.is_map() && current.has_child(name))
            {
                context.parents.push(current[name]);

                return true;
            }

            return false;
        }

        static bool beginMember(auto&& member, const std::size_t i, YamlContext& context)
        {
            return beginMemberGeneric(member, i, context);
        }

        template<class T, class TValue>
        static bool beginMember(reflection::ClassMember<T, std::optional<TValue>>& member, const std::size_t i, YamlContext& context)
        {
            std::optional<TValue> tempValue;
            reflection::ClassMember<T, std::optional<TValue>> tempMember {
                member.name,
                member.pointer,
                tempValue
            };

            if(beginMemberGeneric(tempMember, i, context))
            {
                member.value = TValue{};
                return true;
            }

            return false;
        }

        static void endMember(const bool lastMember, YamlContext& context)
        {
            context.parents.pop();
        }
    };

    struct DeserializerCollectionItemPolicy
    {
        inline static bool beginItem(const std::size_t i, YamlContext& context)
        {
            auto current = context.parents.top();
            if(current.is_seq() && i < current.num_children())
            {
                auto itemNode = current.at(i);
                context.parents.push(itemNode);

                return true;
            }

            return false;
        }

        inline static void endItem(const bool lastItem, YamlContext& context)
        {
            context.parents.pop();
        }
    };

    struct DeserializerContainerPolicy: public ContainerPolicy<DeserializerContainerPolicy, TreeStructureContext<ryml::NodeRef>>
    {
        template<typename TCollection>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static void setCollectionSize(TCollection& collection, std::stack<NodeType>& parents, std::size_t& size)
        {
            auto currentNode = parents.top();
            size = currentNode.is_seq() ? currentNode.num_children() : 0;
        }

        template<typename TCollection>
        requires(generic::is_dictionary_v<TCollection>)
        static void setCollectionSize(TCollection& collection, std::stack<NodeType>& parents, std::size_t& size)
        {
            auto currentNode = parents.top();
            if(currentNode.is_map())
            {
                size = currentNode.num_children();
            }
            else
            {
                size = 0;
            }
        }

        template<typename TCollection>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static auto& getCollectionItem(TCollection& collection, auto& index, std::stack<NodeType>& parents)
        {
            auto currentNode = parents.top();
            auto itemNode = currentNode.at(index);
            parents.push(itemNode);

            return collection.emplace_back(typename TCollection::value_type{});
        }

        template<typename TCollection>
        requires(generic::is_dictionary_v<TCollection>)
        static auto& getCollectionItem(TCollection& collection, auto& index, std::stack<NodeType>& parents)
        {
            using ValueType = typename TCollection::mapped_type;

            auto currentNode = parents.top();

            if(currentNode.is_map())
            {
                auto itemNode = currentNode.at(index);
                parents.push(itemNode);
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

    template<typename T>
    static void deserialize(std::string source, T& object)
    {
        auto tree = ryml::parse_in_place(toRymlStr(source));
        YamlContext context(tree.rootref());
        ObjectVisitor<YamlDeserializationPolicies>::visit(object, context);
    }

    template<typename T>
    static T deserialize(std::string source)
    {
        auto object = T{};

        deserialize(source, object);

        return object;
    }
}