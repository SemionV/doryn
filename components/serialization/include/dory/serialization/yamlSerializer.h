#pragma once

#include "objectVisitor.h"
#include "yamlSerializationContext.h"
#include <magic_enum/magic_enum.hpp>

namespace dory::serialization::yaml
{
    struct SerializerValuePolicy
    {
    private:
        template<typename T>
        static void writeValue(T& value, ryml::NodeRef& node, YamlContext& context)
        {
            node << value;
        }

        static void writeValue(const std::string& value, ryml::NodeRef& node, YamlContext& context)
        {
            node = toRymlCStr(value);
        }

        /*static void writeValue(std::string& value, ryml::NodeRef& node, YamlContext& context)
        {
            node = toRymlStr(value);
        }*/

    public:
        template<typename T>
        inline static void process(T& value, YamlContext& context)
        {
            auto current = context.parents.top();
            current |= c4::yml::NodeType_e::VAL;
            writeValue(value, current, context);
        }
    };

    struct SerializerEnumPolicy
    {
        template<typename T>
        static void process(T& value, YamlContext& context)
        {
            auto current = context.parents.top();
            current |= c4::yml::NodeType_e::VAL;

            auto valueName = magic_enum::enum_name(value);
            current = toRymlCStr(valueName);
        }
    };

    struct SerializerObjectPolicy
    {
        template<typename T>
        inline static bool beginObject(T&& object, YamlContext& context)
        {
            auto current = context.parents.top();
            current |= c4::yml::NodeType_e::MAP;

            return true;
        }

        template<typename T>
        inline static void endObject(T&& object, YamlContext& context)
        {
        }
    };

    struct SerializerMemberPolicy
    {
        static bool beginMemberGeneric(auto&& member, const std::size_t i, YamlContext& context)
        {
            auto current = context.parents.top();

            auto memberNode = current.append_child();
            memberNode.set_key(toRymlCStr(member.name));
            context.parents.push(memberNode);

            return true;
        }

        static bool beginMember(auto&& member, const std::size_t i, YamlContext& context)
        {
            return beginMemberGeneric(member, i, context);
        }

        template<class T, class TValue>
        requires(generic::is_optional_v<std::decay_t<TValue>>)
        static bool beginMember(reflection::ClassMember<T, TValue>& member, const std::size_t i, YamlContext& context)
        {
            if(member.value.has_value())
            {
                return beginMemberGeneric(member, i, context);
            }

            return false;
        }

        static void endMember(const bool lastMember, YamlContext& context)
        {
            context.parents.pop();
        }
    };

    struct SerializerCollectionPolicy
    {
        template<typename T, auto N, typename TCollection>
        inline static void beginCollection(TCollection&& collection, YamlContext& context)
        {
            auto currentNode = context.parents.top();
            currentNode |= c4::yml::NodeType_e::SEQ;
#ifdef DORY_PLATFORM_WIN32
            currentNode |= c4::yml::NodeType_e::FLOW_SL;
#endif
#ifdef DORY_PLATFORM_LINUX
            currentNode |= c4::yml::NodeType_e::FLOW_SL;
#endif
        }

        inline static void endCollection(YamlContext& context)
        {
        }
    };

    struct SerializerCollectionItemPolicy
    {
        inline static bool beginItem(const std::size_t i, YamlContext& context)
        {
            auto current = context.parents.top();
            auto itemNode = current.append_child();
            context.parents.push(itemNode);

            return true;
        }

        inline static void endItem(const bool lastItem, YamlContext& context)
        {
            context.parents.pop();
        }
    };

    struct SerializerContainerPolicy: public ContainerPolicy<SerializerContainerPolicy, TreeStructureContext<ryml::NodeRef>>
    {
        template<typename TCollection>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static void setCollectionSize(TCollection& collection, std::stack<NodeType>& parents, std::size_t& size)
        {
            auto currentNode = parents.top();
            currentNode |= c4::yml::NodeType_e::SEQ;
            size = collection.size();
        }

        template<typename TCollection>
        requires(generic::is_dictionary_v<TCollection>)
        static void setCollectionSize(TCollection& collection, std::stack<NodeType>& parents, std::size_t& size)
        {
            auto currentNode = parents.top();
            currentNode |= c4::yml::NodeType_e::MAP;

            size = collection.size();
        }

        template<typename TCollection>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static auto& getCollectionItem(TCollection& collection, auto& index, std::stack<NodeType>& parents)
        {
            auto currentNode = parents.top();
            auto& item = collection[index];

            const auto itemNode = currentNode.append_child();
            parents.push(itemNode);

           return item;
        }

        template<typename TCollection>
        requires(generic::is_dictionary_v<TCollection>)
        static auto& getCollectionItem(TCollection& collection, auto& index, std::stack<NodeType>& parents)
        {
            auto& item = getMapItem(collection, index);

            auto currentNode = parents.top();
            auto itemNode = currentNode.append_child();
            auto keyString = getKeyString(item.first);
            itemNode.set_key(toRymlCStr(keyString));
            parents.push(itemNode);

            return item.second;
        }
    };

    struct YamlSerializationPolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = SerializerValuePolicy;
        using EnumPolicy = SerializerEnumPolicy;
        using ObjectPolicy = SerializerObjectPolicy;
        using MemberPolicy = SerializerMemberPolicy;
        using CollectionPolicy = SerializerCollectionPolicy;
        using CollectionItemPolicy = SerializerCollectionItemPolicy;
        using ContainerPolicyType = SerializerContainerPolicy;
    };

    template<typename T>
    static std::string serialize(const T& object)
    {
        auto tree = ryml::Tree{};

        YamlContext context(tree.rootref());
        ObjectVisitor<YamlSerializationPolicies>::visit(object, context);

        std::stringstream stream;
        stream << tree;
        return stream.str();
    }
}