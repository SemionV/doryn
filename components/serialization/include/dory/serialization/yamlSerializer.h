#pragma once

#include "objectVisitor.h"
#include "yamlSerializationContext.h"
#include <magic_enum/magic_enum.hpp>

namespace dory::serialization::yaml
{
    struct SerializerValuePolicy
    {
    private:
        template<typename T, typename TContextPolicies>
        static void writeValue(T& value, ryml::NodeRef& node, YamlContext<TContextPolicies>& context)
        {
            node << value;
        }

        template<typename TContextPolicies>
        static void writeValue(const std::string& value, ryml::NodeRef& node, YamlContext<TContextPolicies>& context)
        {
            node = toRymlCStr(value);
        }

    public:
        template<typename T, typename TContextPolicies>
        static void process(T& value, YamlContext<TContextPolicies>& context)
        {
            auto current = context.node;
            current |= c4::yml::NodeType_e::VAL;
            writeValue(value, current, context);
        }
    };

    struct SerializerEnumPolicy
    {
        template<typename T, typename TContextPolicies>
        static void process(T& value, YamlContext<TContextPolicies>& context)
        {
            auto current = context.node;
            current |= c4::yml::NodeType_e::VAL;

            auto valueName = magic_enum::enum_name(value);
            current = toRymlCStr(valueName);
        }
    };

    struct SerializerObjectPolicy
    {
        template<typename T, typename TContextPolicies>
        static bool beginObject(T&& object, YamlContext<TContextPolicies>& context)
        {
            auto current = context.node;
            current |= c4::yml::NodeType_e::MAP;

            return true;
        }

        template<typename T, typename TContextPolicies>
        static void endObject(T&& object, YamlContext<TContextPolicies>& context)
        {
        }
    };

    struct SerializerMemberPolicy
    {
        template<typename TContextPolicies>
        static std::optional<YamlContext<TContextPolicies>> beginMemberGeneric(auto&& member, const std::size_t i, YamlContext<TContextPolicies>& context)
        {
            auto current = context.node;

            auto memberNode = current.append_child();
            memberNode.set_key(toRymlCStr(member.name));

            return YamlContext{ memberNode, context };
        }

        template<typename TContextPolicies>
        static std::optional<YamlContext<TContextPolicies>> beginMember(auto&& member, const std::size_t i, YamlContext<TContextPolicies>& context)
        {
            return beginMemberGeneric(member, i, context);
        }

        template<class T, class TValue, typename TContextPolicies>
        requires(generic::is_optional_v<std::decay_t<TValue>>)
        static std::optional<YamlContext<TContextPolicies>> beginMember(reflection::ClassMember<T, TValue>& member, const std::size_t i, YamlContext<TContextPolicies>& context)
        {
            if(member.value.has_value())
            {
                return beginMemberGeneric(member, i, context);
            }

            return {};
        }

        template<typename TContextPolicies>
        static void endMember(const bool lastMember, YamlContext<TContextPolicies>& context)
        {}
    };

    struct SerializerCollectionPolicy
    {
        template<typename T, auto N, typename TCollection, typename TContextPolicies>
        static void beginCollection(TCollection&& collection, YamlContext<TContextPolicies>& context)
        {
            auto currentNode = context.node;
            currentNode |= c4::yml::NodeType_e::SEQ;
#ifdef DORY_PLATFORM_WIN32
            currentNode |= c4::yml::NodeType_e::FLOW_SL;
#endif
#ifdef DORY_PLATFORM_LINUX
            currentNode |= c4::yml::NodeType_e::FLOW_SL;
#endif
        }

        template<typename TContextPolicies>
        static void endCollection(YamlContext<TContextPolicies>& context)
        {
        }
    };

    struct SerializerCollectionItemPolicy
    {
        template<typename TContextPolicies>
        static std::optional<YamlContext<TContextPolicies>> beginItem(const std::size_t i, const YamlContext<TContextPolicies>& context)
        {
            auto current = context.node;
            const auto itemNode = current.append_child();

            return YamlContext{ itemNode, context };
        }

        template<typename TContextPolicies>
        static void endItem(const bool lastItem, YamlContext<TContextPolicies>& context)
        {}
    };

    struct SerializerContainerPolicy: public ContainerPolicy<SerializerContainerPolicy>
    {
        using NodeType = ryml::NodeRef;

        template<typename TCollection>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static void setCollectionSize(TCollection& collection, NodeType& collectionNode, std::size_t& size)
        {
            collectionNode |= c4::yml::NodeType_e::SEQ;
            size = collection.size();
        }

        template<typename TCollection>
        requires(generic::is_dictionary_v<TCollection>)
        static void setCollectionSize(TCollection& collection, NodeType& collectionNode, std::size_t& size)
        {
            collectionNode |= c4::yml::NodeType_e::MAP;
            size = collection.size();
        }

        template<typename TCollection, typename TItem, typename TContextPolicies>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static std::optional<YamlContext<TContextPolicies>> getCollectionItem(YamlContext<TContextPolicies>& context,
            TCollection& collection, auto& index, NodeType& collectionNode, TItem** item)
        {
            *item = &collection[index];

            const auto itemNode = collectionNode.append_child();
            return YamlContext{ itemNode, context };
        }

        template<typename TCollection, typename TItem, typename TContextPolicies>
        requires(generic::is_dictionary_v<TCollection>)
        static std::optional<YamlContext<TContextPolicies>> getCollectionItem(YamlContext<TContextPolicies>& context,
            TCollection& collection, auto& index, NodeType& collectionNode, TItem** item)
        {
            auto& pair = getMapItem(collection, index);
            *item = &pair.second;

            auto itemNode = collectionNode.append_child();
            auto keyString = getKeyString(pair.first);
            itemNode.set_key(toRymlCStr(keyString));

            return YamlContext { itemNode, context };
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

    template<typename T, typename TContextPolicies, typename... TVisitorBases>
    static std::string serialize(const T& object, generic::serialization::Context<TContextPolicies>&& contextBase)
    {
        auto tree = ryml::Tree{};

        YamlContext<TContextPolicies> context(tree.rootref(), contextBase);
        ObjectVisitor<YamlSerializationPolicies, TVisitorBases...>::visit(object, context);

        std::stringstream stream;
        stream << tree;
        return stream.str();
    }
}