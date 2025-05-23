#pragma once

#include "objectVisitor.h"
#include "jsonSerializationContext.h"

namespace dory::serialization::json
{
    struct SerializerValuePolicy
    {
        template<typename T, typename TContextPolicies>
        static void process(const T& value, JsonContext<TContextPolicies>& context)
        {
            *context.node = value;
        }
    };

    struct SerializerEnumPolicy
    {
        template<typename T, typename TContextPolicies>
        static void process(T& value, JsonContext<TContextPolicies>& context)
        {
            *context.node = magic_enum::enum_name(value);
        }
    };

    struct SerializerObjectPolicy
    {
        template<typename T, typename TContextPolicies>
        static bool beginObject(T&& object, JsonContext<TContextPolicies>& context)
        {
            *context.node = json::object();
            return true;
        }

        template<typename T, typename TContextPolicies>
        static void endObject(T&& object, JsonContext<TContextPolicies>& context)
        {
        }
    };

    struct SerializerMemberPolicy
    {
        template<typename TContextPolicies>
        static std::optional<JsonContext<TContextPolicies>> beginMemberGeneric(auto&& member, const std::size_t i, JsonContext<TContextPolicies>& context)
        {
            auto& memberJson = context.node->operator[](member.name);
            return JsonContext{ &memberJson, context };
        }

        template<typename TContextPolicies>
        static std::optional<JsonContext<TContextPolicies>> beginMember(auto&& member, const std::size_t i, JsonContext<TContextPolicies>& context)
        {
            return beginMemberGeneric(member, i, context);
        }

        template<class T, class TValue, typename TContextPolicies>
        requires(generic::is_optional_v<std::decay_t<TValue>>)
        static std::optional<JsonContext<TContextPolicies>> beginMember(reflection::ClassMember<T, TValue>& member, const std::size_t i, JsonContext<TContextPolicies>& context)
        {
            if(member.value.has_value())
            {
                return beginMemberGeneric(member, i, context);
            }

            return {};
        }

        template<typename TContextPolicies>
        static void endMember(const bool lastMember, JsonContext<TContextPolicies>& context)
        {}
    };

    struct SerializerCollectionPolicy
    {
        template<typename T, auto N, typename TCollection, typename TContextPolicies>
        static void beginCollection(TCollection&& collection, JsonContext<TContextPolicies>& context)
        {
            *context.node = json::array();
        }

        template<typename TContextPolicies>
        static void endCollection(JsonContext<TContextPolicies>& context)
        {
        }
    };

    struct SerializerCollectionItemPolicy
    {
        template<typename TContextPolicies>
        static std::optional<JsonContext<TContextPolicies>> beginItem(const std::size_t i, const JsonContext<TContextPolicies>& context)
        {
            auto& itemJson = context.node->operator[](i);
            return JsonContext{ &itemJson, context };
        }

        template<typename TContextPolicies>
        static void endItem(const bool lastItem, JsonContext<TContextPolicies>& context)
        {}
    };

    struct SerializerContainerPolicy: public ContainerPolicy<SerializerContainerPolicy>
    {
        using NodeType = json*;

        template<typename TCollection>
        static void setCollectionSize(TCollection& collection, NodeType& collectionNode, std::size_t& size)
        {
            size = collection.size();
        }

        template<typename TCollection, typename TItem, typename TContextPolicies>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static std::optional<JsonContext<TContextPolicies>> getCollectionItem(JsonContext<TContextPolicies>& context,
            TCollection& collection, auto& index, NodeType& collectionNode, TItem** item)
        {
            *item = &collection[index];

            auto& itemJson = collectionNode->operator[](index);
            return JsonContext{ &itemJson, context };
        }

        template<typename TCollection, typename TItem, typename TContextPolicies>
        requires(generic::is_dictionary_v<TCollection>)
        static std::optional<JsonContext<TContextPolicies>> getCollectionItem(JsonContext<TContextPolicies>& context,
            TCollection& collection, auto& index, NodeType& collectionNode, TItem** item)
        {
            auto& pair = getMapItem(collection, index);
            *item = &pair.second;

            auto& itemJson = collectionNode->operator[](getKeyString(pair.first));
            return JsonContext{ &itemJson, context };
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

    template<typename T, typename TContextPolicies, typename... TVisitorBases>
    static std::string serialize(const T& object, generic::serialization::Context<TContextPolicies>&& contextBase, const int indent = -1)
    {
        auto data = json{};
        JsonContext<TContextPolicies> context(&data, contextBase);
        ObjectVisitor<JsonSerializationPolicies, TVisitorBases...>::visit(object, context);

        return data.dump(indent);
    }
}
