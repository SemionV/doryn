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

    public:
        template<typename T>
        static void process(T& value, YamlContext& context)
        {
            auto current = context.node;
            current |= c4::yml::NodeType_e::VAL;
            writeValue(value, current, context);
        }
    };

    struct SerializerEnumPolicy
    {
        template<typename T>
        static void process(T& value, YamlContext& context)
        {
            auto current = context.node;
            current |= c4::yml::NodeType_e::VAL;

            auto valueName = magic_enum::enum_name(value);
            current = toRymlCStr(valueName);
        }
    };

    struct SerializerObjectPolicy
    {
        template<typename T>
        static bool beginObject(T&& object, YamlContext& context)
        {
            auto current = context.node;
            current |= c4::yml::NodeType_e::MAP;

            return true;
        }

        template<typename T>
        static void endObject(T&& object, YamlContext& context)
        {
        }
    };

    struct SerializerMemberPolicy
    {
        static std::optional<YamlContext> beginMemberGeneric(auto&& member, const std::size_t i, YamlContext& context)
        {
            auto current = context.node;

            auto memberNode = current.append_child();
            memberNode.set_key(toRymlCStr(member.name));

            return YamlContext{ memberNode };
        }

        static std::optional<YamlContext> beginMember(auto&& member, const std::size_t i, YamlContext& context)
        {
            return beginMemberGeneric(member, i, context);
        }

        template<class T, class TValue>
        requires(generic::is_optional_v<std::decay_t<TValue>>)
        static std::optional<YamlContext> beginMember(reflection::ClassMember<T, TValue>& member, const std::size_t i, YamlContext& context)
        {
            if(member.value.has_value())
            {
                return beginMemberGeneric(member, i, context);
            }

            return {};
        }

        static void endMember(const bool lastMember, YamlContext& context)
        {}
    };

    struct SerializerCollectionPolicy
    {
        template<typename T, auto N, typename TCollection>
        static void beginCollection(TCollection&& collection, YamlContext& context)
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

        static void endCollection(YamlContext& context)
        {
        }
    };

    struct SerializerCollectionItemPolicy
    {
        static std::optional<YamlContext> beginItem(const std::size_t i, const YamlContext& context)
        {
            auto current = context.node;
            const auto itemNode = current.append_child();

            return YamlContext{ itemNode };
        }

        static void endItem(const bool lastItem, YamlContext& context)
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

        template<typename TCollection, typename TItem>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static std::optional<YamlContext> getCollectionItem(TCollection& collection, auto& index, NodeType& collectionNode, TItem** item)
        {
            *item = &collection[index];

            const auto itemNode = collectionNode.append_child();
            return YamlContext{ itemNode };
        }

        template<typename TCollection, typename TItem>
        requires(generic::is_dictionary_v<TCollection>)
        static std::optional<YamlContext> getCollectionItem(TCollection& collection, auto& index, NodeType& collectionNode, TItem** item)
        {
            auto& pair = getMapItem(collection, index);
            *item = &pair.second;

            auto itemNode = collectionNode.append_child();
            auto keyString = getKeyString(pair.first);
            itemNode.set_key(toRymlCStr(keyString));

            return YamlContext { itemNode };
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

    template<typename T, typename... TVisitorBases>
    static std::string serialize(const T& object)
    {
        auto tree = ryml::Tree{};

        YamlContext context(tree.rootref());
        ObjectVisitor<YamlSerializationPolicies, TVisitorBases...>::visit(object, context);

        std::stringstream stream;
        stream << tree;
        return stream.str();
    }
}