#pragma once

#include "objectVisitor.h"
#include "jsonSerializationContext.h"

namespace dory::serialization::json
{
    struct SerializerValuePolicy
    {
        template<typename T, typename TRegistry, typename TDataContext>
        static void process(const T& value, JsonContext<TRegistry, TDataContext>& context)
        {
            *context.node = value;
        }
    };

    struct SerializerEnumPolicy
    {
        template<typename T, typename TRegistry, typename TDataContext>
        static void process(T& value, JsonContext<TRegistry, TDataContext>& context)
        {
            *context.node = magic_enum::enum_name(value);
        }
    };

    struct SerializerObjectPolicy
    {
        template<typename T, typename TRegistry, typename TDataContext>
        static bool beginObject(T&& object, JsonContext<TRegistry, TDataContext>& context)
        {
            *context.node = json::object();
            return true;
        }

        template<typename T, typename TRegistry, typename TDataContext>
        static void endObject(T&& object, JsonContext<TRegistry, TDataContext>& context)
        {
        }
    };

    struct SerializerMemberPolicy
    {
        template<typename TRegistry, typename TDataContext>
        static std::optional<JsonContext<TRegistry, TDataContext>> beginMemberGeneric(auto&& member, const std::size_t i, JsonContext<TRegistry, TDataContext>& context)
        {
            auto& memberJson = context.node->operator[](member.name);
            return JsonContext{ &memberJson, context.registry, context.dataContext };
        }

        template<typename TRegistry, typename TDataContext>
        static std::optional<JsonContext<TRegistry, TDataContext>> beginMember(auto&& member, const std::size_t i, JsonContext<TRegistry, TDataContext>& context)
        {
            return beginMemberGeneric(member, i, context);
        }

        template<class T, class TValue, typename TRegistry, typename TDataContext>
        requires(generic::is_optional_v<std::decay_t<TValue>>)
        static std::optional<JsonContext<TRegistry, TDataContext>> beginMember(reflection::ClassMember<T, TValue>& member, const std::size_t i, JsonContext<TRegistry, TDataContext>& context)
        {
            if(member.value.has_value())
            {
                return beginMemberGeneric(member, i, context);
            }

            return {};
        }

        template<typename TRegistry, typename TDataContext>
        static void endMember(const bool lastMember, JsonContext<TRegistry, TDataContext>& context)
        {}
    };

    struct SerializerCollectionPolicy
    {
        template<typename T, auto N, typename TCollection, typename TRegistry, typename TDataContext>
        static void beginCollection(TCollection&& collection, JsonContext<TRegistry, TDataContext>& context)
        {
            *context.node = json::array();
        }

        template<typename TRegistry, typename TDataContext>
        static void endCollection(JsonContext<TRegistry, TDataContext>& context)
        {
        }
    };

    struct SerializerCollectionItemPolicy
    {
        template<typename TRegistry, typename TDataContext>
        static std::optional<JsonContext<TRegistry, TDataContext>> beginItem(const std::size_t i, const JsonContext<TRegistry, TDataContext>& context)
        {
            auto& itemJson = context.node->operator[](i);
            return JsonContext{ &itemJson, context.registry, context.dataContext };
        }

        template<typename TRegistry, typename TDataContext>
        static void endItem(const bool lastItem, JsonContext<TRegistry, TDataContext>& context)
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

        template<typename TCollection, typename TItem, typename TRegistry, typename TDataContext>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static std::optional<JsonContext<TRegistry, TDataContext>> getCollectionItem(JsonContext<TRegistry, TDataContext>& context,
            TCollection& collection, auto& index, NodeType& collectionNode, TItem** item)
        {
            *item = &collection[index];

            auto& itemJson = collectionNode->operator[](index);
            return JsonContext{ &itemJson, context.registry, context.dataContext };
        }

        template<typename TCollection, typename TItem, typename TRegistry, typename TDataContext>
        requires(generic::is_dictionary_v<TCollection>)
        static std::optional<JsonContext<TRegistry, TDataContext>> getCollectionItem(JsonContext<TRegistry, TDataContext>& context,
            TCollection& collection, auto& index, NodeType& collectionNode, TItem** item)
        {
            auto& pair = getMapItem(collection, index);
            *item = &pair.second;

            auto& itemJson = collectionNode->operator[](getKeyString(pair.first));
            return JsonContext{ &itemJson, context.registry, context.dataContext };
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

    template<typename T, typename TRegistry, typename TDataContext, typename... TVisitorBases>
    static std::string serialize(const T& object, TRegistry& registry, TDataContext& dataContext, const int indent = -1)
    {
        auto data = json{};
        JsonContext<TRegistry, TDataContext> context(&data, registry, dataContext);
        ObjectVisitor<JsonSerializationPolicies, TVisitorBases...>::visit(object, context);

        return data.dump(indent);
    }
}
