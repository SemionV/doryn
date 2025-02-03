#pragma once

#include "objectVisitor.h"
#include "yamlSerializationContext.h"
#include <magic_enum/magic_enum.hpp>

namespace dory::serialization::yaml
{
    struct SerializerValuePolicy
    {
    private:
        template<typename T, typename TRegistry, typename TDataContext>
        static void writeValue(T& value, ryml::NodeRef& node, YamlContext<TRegistry, TDataContext>& context)
        {
            node << value;
        }

        template<typename TRegistry, typename TDataContext>
        static void writeValue(const std::string& value, ryml::NodeRef& node, YamlContext<TRegistry, TDataContext>& context)
        {
            node = toRymlCStr(value);
        }

    public:
        template<typename T, typename TRegistry, typename TDataContext>
        static void process(T& value, YamlContext<TRegistry, TDataContext>& context)
        {
            auto current = context.node;
            current |= c4::yml::NodeType_e::VAL;
            writeValue(value, current, context);
        }
    };

    struct SerializerEnumPolicy
    {
        template<typename T, typename TRegistry, typename TDataContext>
        static void process(T& value, YamlContext<TRegistry, TDataContext>& context)
        {
            auto current = context.node;
            current |= c4::yml::NodeType_e::VAL;

            auto valueName = magic_enum::enum_name(value);
            current = toRymlCStr(valueName);
        }
    };

    struct SerializerObjectPolicy
    {
        template<typename T, typename TRegistry, typename TDataContext>
        static bool beginObject(T&& object, YamlContext<TRegistry, TDataContext>& context)
        {
            auto current = context.node;
            current |= c4::yml::NodeType_e::MAP;

            return true;
        }

        template<typename T, typename TRegistry, typename TDataContext>
        static void endObject(T&& object, YamlContext<TRegistry, TDataContext>& context)
        {
        }
    };

    struct SerializerMemberPolicy
    {
        template<typename TRegistry, typename TDataContext>
        static std::optional<YamlContext<TRegistry, TDataContext>> beginMemberGeneric(auto&& member, const std::size_t i, YamlContext<TRegistry, TDataContext>& context)
        {
            auto current = context.node;

            auto memberNode = current.append_child();
            memberNode.set_key(toRymlCStr(member.name));

            return YamlContext{ memberNode, context.registry, context.dataContext };
        }

        template<typename TRegistry, typename TDataContext>
        static std::optional<YamlContext<TRegistry, TDataContext>> beginMember(auto&& member, const std::size_t i, YamlContext<TRegistry, TDataContext>& context)
        {
            return beginMemberGeneric(member, i, context);
        }

        template<class T, class TValue, typename TRegistry, typename TDataContext>
        requires(generic::is_optional_v<std::decay_t<TValue>>)
        static std::optional<YamlContext<TRegistry, TDataContext>> beginMember(reflection::ClassMember<T, TValue>& member, const std::size_t i, YamlContext<TRegistry, TDataContext>& context)
        {
            if(member.value.has_value())
            {
                return beginMemberGeneric(member, i, context);
            }

            return {};
        }

        template<typename TRegistry, typename TDataContext>
        static void endMember(const bool lastMember, YamlContext<TRegistry, TDataContext>& context)
        {}
    };

    struct SerializerCollectionPolicy
    {
        template<typename T, auto N, typename TCollection, typename TRegistry, typename TDataContext>
        static void beginCollection(TCollection&& collection, YamlContext<TRegistry, TDataContext>& context)
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

        template<typename TRegistry, typename TDataContext>
        static void endCollection(YamlContext<TRegistry, TDataContext>& context)
        {
        }
    };

    struct SerializerCollectionItemPolicy
    {
        template<typename TRegistry, typename TDataContext>
        static std::optional<YamlContext<TRegistry, TDataContext>> beginItem(const std::size_t i, const YamlContext<TRegistry, TDataContext>& context)
        {
            auto current = context.node;
            const auto itemNode = current.append_child();

            return YamlContext{ itemNode, context.registry, context.dataContext };
        }

        template<typename TRegistry, typename TDataContext>
        static void endItem(const bool lastItem, YamlContext<TRegistry, TDataContext>& context)
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

        template<typename TCollection, typename TItem, typename TRegistry, typename TDataContext>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static std::optional<YamlContext<TRegistry, TDataContext>> getCollectionItem(YamlContext<TRegistry, TDataContext>& context,
            TCollection& collection, auto& index, NodeType& collectionNode, TItem** item)
        {
            *item = &collection[index];

            const auto itemNode = collectionNode.append_child();
            return YamlContext{ itemNode, context.registry, context.dataContext };
        }

        template<typename TCollection, typename TItem, typename TRegistry, typename TDataContext>
        requires(generic::is_dictionary_v<TCollection>)
        static std::optional<YamlContext<TRegistry, TDataContext>> getCollectionItem(YamlContext<TRegistry, TDataContext>& context,
            TCollection& collection, auto& index, NodeType& collectionNode, TItem** item)
        {
            auto& pair = getMapItem(collection, index);
            *item = &pair.second;

            auto itemNode = collectionNode.append_child();
            auto keyString = getKeyString(pair.first);
            itemNode.set_key(toRymlCStr(keyString));

            return YamlContext { itemNode, context.registry, context.dataContext };
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

    template<typename T, typename TRegistry, typename TDataContext, typename... TVisitorBases>
    static std::string serialize(const T& object, TRegistry& registry, TDataContext& dataContext)
    {
        auto tree = ryml::Tree{};

        YamlContext<TRegistry, TDataContext> context(tree.rootref(), registry, dataContext);
        ObjectVisitor<YamlSerializationPolicies, TVisitorBases...>::visit(object, context);

        std::stringstream stream;
        stream << tree;
        return stream.str();
    }
}