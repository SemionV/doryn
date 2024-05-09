#pragma once

#include "objectVisitor.h"
#include "jsonSerializationContext.h"
#include "base/dependencies.h"

namespace dory::typeMap::json
{
    struct SerializerValuePolicy
    {
        template<typename T>
        inline static void process(const T& value, JsonContext& context)
        {
            auto* currentJson = context.current.top();
            *currentJson = value;
        }
    };

    struct SerializerObjectPolicy
    {
        inline static void beginObject(JsonContext& context)
        {
            auto* currentJson = context.current.top();
            *currentJson = json::object();
        }

        inline static void endObject(JsonContext& context)
        {
        }
    };

    struct SerializerMemberPolicy
    {
        inline static void beginMember(const std::string& memberName, const std::size_t i, JsonContext& context)
        {
            auto* currentJson = context.current.top();

            auto& memberJson = currentJson->operator[](memberName);
            context.current.push(&memberJson);
        }

        inline static void endMember(const bool lastMember, JsonContext& context)
        {
            context.current.pop();
        }
    };

    struct SerializerCollectionPolicy
    {
        template<typename T, auto N>
        inline static void beginCollection(JsonContext& context)
        {
            auto* currentJson = context.current.top();
            *currentJson = json::array();
        }

        inline static void endCollection(JsonContext& context)
        {
        }
    };

    struct SerializerCollectionItemPolicy
    {
        inline static void beginItem(const std::size_t i, JsonContext& context)
        {
            auto* currentJson = context.current.top();
            auto& itemJson = currentJson->operator[](i);
            context.current.push(&itemJson);
        }

        inline static void endItem(const bool lastItem, JsonContext& context)
        {
            context.current.pop();
        }
    };

    struct SerializerDynamicCollectionPolicy
    {
        template<typename T>
        inline static void beginCollection(std::vector<T>& collection, JsonContext& context)
        {
            context.previousDynamicCollectionIndex = context.dynamicCollectionIndex;
            context.dynamicCollectionIndex = 0;
        }

        template<typename T>
        inline static std::optional<T> getNextItem(std::vector<T>& collection, JsonContext& context)
        {
            auto* currentJson = context.current.top();
            if(context.dynamicCollectionIndex < collection.size())
            {
                auto& item = collection[context.dynamicCollectionIndex];

                auto& itemJson = currentJson->operator[](context.dynamicCollectionIndex);
                context.current.push(&itemJson);

                ++context.dynamicCollectionIndex;

                return item;
            }

            return {};
        }

        template<typename T>
        inline static void processItem(T& item, std::vector<T>& collection, JsonContext& context)
        {
            context.current.pop();
        }

        template<typename T>
        inline static void endCollection(std::vector<T>& collection, JsonContext& context)
        {
            context.dynamicCollectionIndex = context.previousDynamicCollectionIndex;
            context.previousDynamicCollectionIndex = 0;
        }
    };

    struct JsonSerializationPolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = SerializerValuePolicy;
        using ObjectPolicy = SerializerObjectPolicy;
        using MemberPolicy = SerializerMemberPolicy;
        using CollectionPolicy = SerializerCollectionPolicy;
        using CollectionItemPolicy = SerializerCollectionItemPolicy;
        using DynamicCollectionPolicyType = SerializerDynamicCollectionPolicy;
    };

    class JsonSerializer
    {
    public:
        template<typename T>
        static std::string serialize(T& object, const int indent = -1)
        {
            auto data = json{};
            JsonContext context(&data);
            ObjectVisitor<JsonSerializationPolicies>::visit(object, context);

            return data.dump(indent);
        }
    };
}