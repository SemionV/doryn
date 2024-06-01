#pragma once

#include "objectVisitor.h"
#include "yamlSerializationContext.h"
#include "base/dependencies.h"

namespace dory::typeMap::yaml
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
            value = std::string(node.val().str, node.val().len);
        }

    public:
        template<typename T>
        inline static void process(T& value, YamlContext& context)
        {
            auto current = context.current.top();
            if(current.has_val())
            {
                readValue(value, current);
            }
        }
    };

    struct DeserializerMemberPolicy
    {
        template<class T>
        inline static bool beginMember(const std::string_view& memberName, const T& value, const std::size_t i, YamlContext& context)
        {
            auto current = context.current.top();
            const auto& name = toRymlCStr(memberName);
            if(current.is_map() && current.has_child(name))
            {
                auto member = current[name];
                context.current.push(member);

                return true;
            }

            return false;
        }

        template<class T>
        inline static bool beginMember(const std::string_view& memberName, std::optional<T>& value, const std::size_t i, YamlContext& context)
        {
            if(beginMember(memberName, *value, i, context))
            {
                value = T{};
                return true;
            }

            return false;
        }

        inline static void endMember(const bool lastMember, YamlContext& context)
        {
            context.current.pop();
        }
    };

    struct DeserializerCollectionItemPolicy
    {
        inline static bool beginItem(const std::size_t i, YamlContext& context)
        {
            auto current = context.current.top();
            if(current.is_seq() && i < current.num_children())
            {
                auto itemNode = current.at(i);
                context.current.push(itemNode);

                return true;
            }

            return false;
        }

        inline static void endItem(const bool lastItem, YamlContext& context)
        {
            context.current.pop();
        }
    };

    struct DeserializerContainerPolicy: public ContainerPolicy<DeserializerContainerPolicy, TreeStructureContext<ryml::NodeRef>>
    {
        using NodeType = ryml::NodeRef;

        template<typename TCollection>
        inline static void setCollectionSize(TCollection& collection, ContextType& context, std::size_t& size)
        {
            auto current = context.current.top();
            size = current.is_seq() ? current.num_children() : 0;
        }

        template<typename TCollection, typename TKeysContainer>
        inline static void buildDictionaryKeysList(TCollection& collection, ContextType& context, TKeysContainer& keys)
        {
            auto currentNode = context.current.top();

            if(currentNode.is_map())
            {
                size_t count = currentNode.num_children();
                for(std::size_t i = 0; i < count; ++i)
                {
                    auto itemNode = currentNode.at(i);
                    auto nodeKey = itemNode.key();
                    auto key = std::string(nodeKey.str, nodeKey.len);

                    keys.emplace(key);
                }
            }
        }

        template<typename TCollection>
        inline static auto& getCollectionItem(TCollection& collection, auto& index, std::stack<NodeType>& parents)
        {
            auto currentNode = parents.top();
            auto itemNode = currentNode.at(index);
            parents.push(itemNode);

            return collection.emplace_back(typename TCollection::value_type{});
        }

        template<typename TCollection>
        inline static auto& getDictionaryItem(TCollection& collection, const auto& key, std::stack<NodeType>& parents)
        {
            auto currentNode = parents.top();
            auto itemNode = currentNode.at(toRymlCStr(key));
            parents.push(itemNode);

            if(collection.contains(key))
            {
                return collection[key];
            }

            return collection[key] = typename TCollection::mapped_type{};
        }
    };

    struct YamlDeserializationPolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = DeserializerValuePolicy;
        using MemberPolicy = DeserializerMemberPolicy;
        using CollectionItemPolicy = DeserializerCollectionItemPolicy;
        using ContainerPolicyType = DeserializerContainerPolicy;
    };

    class YamlDeserializer
    {
    public:
        template<typename T>
        static T deserialize(std::string source, T& object)
        {
            auto tree = ryml::parse_in_place(toRymlStr(source));
            YamlContext context(tree.rootref());
            ObjectVisitor<YamlDeserializationPolicies>::visit(object, context);

            return object;
        }

        template<typename T>
        static T deserialize(std::string source)
        {
            auto object = T{};

            deserialize(source, object);

            return object;
        }
    };
}