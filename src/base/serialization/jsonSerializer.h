#pragma once

#include "objectVisitor.h"
#include "jsonSerializationContext.h"
#include "base/dependencies.h"

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
        template<class T>
        inline static bool beginMember(const std::string_view& memberName, T& value, const std::size_t i, JsonContext& context)
        {
            auto* currentJson = context.parents.top();

            auto& memberJson = currentJson->operator[](memberName);
            context.parents.push(&memberJson);

            return true;
        }

        template<class T>
        inline static bool beginMember(const std::string_view& memberName, const std::optional<T>& value, const std::size_t i, JsonContext& context)
        {
            if(value.has_value())
            {
                return beginMember(memberName, *value, i, context);
            }

            return false;
        }

        inline static void endMember(const bool lastMember, JsonContext& context)
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
        inline static void setCollectionSize(TCollection& collection, std::stack<NodeType>& parents, std::size_t& size)
        {
            size = collection.size();
        }

        template<typename TCollection, typename TKeysContainer>
        inline static void buildDictionaryKeysList(TCollection& collection, std::stack<NodeType>& parents, TKeysContainer& keys)
        {
            for(auto& pair : collection)
            {
                keys.emplace(pair.first);
            }
        }

        template<typename TCollection>
        inline static auto& getCollectionItem(TCollection& collection, auto& index, std::stack<NodeType>& parents)
        {
            auto currentNode = parents.top();
            auto& item = collection[index];

            auto& itemJson = currentNode->operator[](index);
            parents.push(&itemJson);

            return item;
        }

        template<typename TCollection>
        inline static auto& getDictionaryItem(TCollection& collection, const auto& key, std::stack<NodeType>& parents)
        {
            auto currentNode = parents.top();

            auto& itemNode = currentNode->operator[](key);
            parents.push(&itemNode);

            return collection.at(key);
        }
    };

    struct JsonSerializationPolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = SerializerValuePolicy;
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
