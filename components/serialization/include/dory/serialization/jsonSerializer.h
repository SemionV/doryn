#pragma once

#include "objectVisitor.h"
#include "jsonSerializationContext.h"

namespace dory::serialization::json
{
    struct SerializerValuePolicy
    {
        template<typename T>
        inline static void process(const T& value, JsonContext& context)
        {
            auto* currentJson = context.parents.top();
            *currentJson = value;
        }
    };

    struct SerializerEnumPolicy
    {
        template<typename T>
        static void process(T& value, JsonContext& context)
        {
            auto current = context.parents.top();
            *current = magic_enum::enum_name(value);
        }
    };

    struct SerializerObjectPolicy
    {
        template<typename T>
        inline static bool beginObject(T&& object, JsonContext& context)
        {
            auto* currentJson = context.parents.top();
            *currentJson = json::object();

            return true;
        }

        template<typename T>
        inline static void endObject(T&& object, JsonContext& context)
        {
        }
    };

    struct SerializerMemberPolicy
    {
        static bool beginMemberGeneric(auto&& member, const std::size_t i, JsonContext& context)
        {
            auto* currentJson = context.parents.top();

            auto& memberJson = currentJson->operator[](member.name);
            context.parents.push(&memberJson);

            return true;
        }

        static bool beginMember(auto&& member, const std::size_t i, JsonContext& context)
        {
            return beginMemberGeneric(member, i, context);
        }

        template<class T, class TValue>
        requires(generic::is_optional_v<std::decay_t<TValue>>)
        static bool beginMember(reflection::ClassMember<T, TValue>& member, const std::size_t i, JsonContext& context)
        {
            if(member.value.has_value())
            {
                return beginMemberGeneric(member, i, context);
            }

            return false;
        }

        static void endMember(const bool lastMember, JsonContext& context)
        {
            context.parents.pop();
        }
    };

    struct SerializerCollectionPolicy
    {
        template<typename T, auto N>
        inline static void beginCollection(JsonContext& context)
        {
            auto* currentJson = context.parents.top();
            *currentJson = json::array();
        }

        inline static void endCollection(JsonContext& context)
        {
        }
    };

    struct SerializerCollectionItemPolicy
    {
        inline static bool beginItem(const std::size_t i, JsonContext& context)
        {
            auto* currentJson = context.parents.top();
            auto& itemJson = currentJson->operator[](i);
            context.parents.push(&itemJson);

            return true;
        }

        inline static void endItem(const bool lastItem, JsonContext& context)
        {
            context.parents.pop();
        }
    };

    struct SerializerContainerPolicy: public ContainerPolicy<SerializerContainerPolicy, TreeStructureContext<json*>>
    {
        template<typename TCollection>
        static void setCollectionSize(TCollection& collection, std::stack<NodeType>& parents, std::size_t& size)
        {
            size = collection.size();
        }

        template<typename TCollection>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static auto& getCollectionItem(TCollection& collection, auto& index, std::stack<NodeType>& parents)
        {
            auto currentNode = parents.top();
            auto& item = collection[index];

            auto& itemJson = currentNode->operator[](index);
            parents.push(&itemJson);

            return item;
        }

        template<typename TCollection>
        requires(generic::is_dictionary_v<TCollection>)
        static auto& getCollectionItem(TCollection& collection, auto& index, std::stack<NodeType>& parents)
        {
            auto& item = getMapItem(collection, index);

            auto currentNode = parents.top();
            auto& itemNode = currentNode->operator[](getKeyString(item.first));
            parents.push(&itemNode);

            return item.second;
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

    template<typename T>
    static std::string serialize(const T& object, const int indent = -1)
    {
        auto data = json{};
        JsonContext context(&data);
        ObjectVisitor<JsonSerializationPolicies>::visit(object, context);

        return data.dump(indent);
    }
}
