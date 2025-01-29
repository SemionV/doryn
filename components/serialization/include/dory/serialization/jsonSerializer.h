#pragma once

#include "objectVisitor.h"
#include "jsonSerializationContext.h"

namespace dory::serialization::json
{
    struct SerializerValuePolicy
    {
        template<typename T>
        static void process(const T& value, JsonContext& context)
        {
            *context.node = value;
        }
    };

    struct SerializerEnumPolicy
    {
        template<typename T>
        static void process(T& value, JsonContext& context)
        {
            *context.node = magic_enum::enum_name(value);
        }
    };

    struct SerializerObjectPolicy
    {
        template<typename T>
        static bool beginObject(T&& object, JsonContext& context)
        {
            *context.node = json::object();
            return true;
        }

        template<typename T>
        static void endObject(T&& object, JsonContext& context)
        {
        }
    };

    struct SerializerMemberPolicy
    {
        static std::optional<JsonContext> beginMemberGeneric(auto&& member, const std::size_t i, JsonContext& context)
        {
            auto& memberJson = context.node->operator[](member.name);
            return JsonContext{ memberJson };
        }

        static std::optional<JsonContext> beginMember(auto&& member, const std::size_t i, JsonContext& context)
        {
            return beginMemberGeneric(member, i, context);
        }

        template<class T, class TValue>
        requires(generic::is_optional_v<std::decay_t<TValue>>)
        static std::optional<JsonContext> beginMember(reflection::ClassMember<T, TValue>& member, const std::size_t i, JsonContext& context)
        {
            if(member.value.has_value())
            {
                return beginMemberGeneric(member, i, context);
            }

            return {};
        }

        static void endMember(const bool lastMember, JsonContext& context)
        {}
    };

    struct SerializerCollectionPolicy
    {
        template<typename T, auto N, typename TCollection>
        static void beginCollection(TCollection&& collection, JsonContext& context)
        {
            *context.node = json::array();
        }

        static void endCollection(JsonContext& context)
        {
        }
    };

    struct SerializerCollectionItemPolicy
    {
        static std::optional<JsonContext> beginItem(const std::size_t i, const JsonContext& context)
        {
            auto& itemJson = context.node->operator[](i);
            return JsonContext{ &itemJson };
        }

        static void endItem(const bool lastItem, JsonContext& context)
        {}
    };

    struct SerializerContainerPolicy: public ContainerPolicy<SerializerContainerPolicy>
    {
        using NodeType = json*;
        using ContextType = TreeStructureContext<NodeType>;

        template<typename TCollection>
        static void setCollectionSize(TCollection& collection, NodeType& collectionNode, std::size_t& size)
        {
            size = collection.size();
        }

        template<typename TCollection, typename TItem>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static std::optional<JsonContext> getCollectionItem(TCollection& collection, auto& index, NodeType& collectionNode, TItem** item)
        {
            *item = &collection[index];

            auto& itemJson = collectionNode->operator[](index);
            return JsonContext{ &itemJson };
        }

        template<typename TCollection, typename TItem>
        requires(generic::is_dictionary_v<TCollection>)
        static std::optional<JsonContext> getCollectionItem(TCollection& collection, auto& index, NodeType& collectionNode, TItem** item)
        {
            auto& pair = getMapItem(collection, index);
            *item = &pair.second;

            auto& itemJson = collectionNode->operator[](getKeyString(pair.first));
            return JsonContext{ &itemJson };
        }
    };

    struct JsonSerializationPolicies: public VisitorDefaultPolicies
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
    static std::string serialize(const T& object, const int indent = -1)
    {
        auto data = json{};
        JsonContext context(&data);
        ObjectVisitor<JsonSerializationPolicies, TVisitorBases...>::visit(object, context);

        return data.dump(indent);
    }
}
