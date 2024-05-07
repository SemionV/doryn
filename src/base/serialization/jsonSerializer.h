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

    struct SerializerBeginObjectPolicy
    {
        inline static void process(JsonContext& context)
        {
            auto* currentJson = context.current.top();
            *currentJson = json::object();
        }
    };

    struct SerializerEndObjectPolicy
    {
        inline static void process(JsonContext& context)
        {
        }
    };

    struct SerializerBeginMemberPolicy
    {
        inline static void process(const std::string& memberName, const std::size_t i, JsonContext& context)
        {
            auto* currentJson = context.current.top();

            auto& memberJson = currentJson->operator[](memberName);
            context.current.push(&memberJson);
        }
    };

    struct SerializerEndMemberPolicy
    {
        inline static void process(const bool lastMember, JsonContext& context)
        {
            context.current.pop();
        }
    };

    struct SerializerBeginCollectionPolicy
    {
        template<typename T, auto N>
        inline static void process(JsonContext& context)
        {
            auto* currentJson = context.current.top();
            *currentJson = json::array();
        }
    };

    struct SerializerEndCollectionPolicy
    {
        inline static void process(JsonContext& context)
        {
        }
    };

    struct SerializerBeginCollectionItemPolicy
    {
        inline static void process(const std::size_t i, JsonContext& context)
        {
            auto* currentJson = context.current.top();
            auto& itemJson = currentJson->operator[](i);
            context.current.push(&itemJson);
        }
    };

    struct SerializerEndCollectionItemPolicy
    {
        inline static void process(const bool lastItem, JsonContext& context)
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
        using BeginObjectPolicy = SerializerBeginObjectPolicy;
        using EndObjectPolicy = SerializerEndObjectPolicy;
        using BeginMemberPolicy = SerializerBeginMemberPolicy;
        using EndMemberPolicy = SerializerEndMemberPolicy;
        using BeginCollectionPolicy = SerializerBeginCollectionPolicy;
        using EndCollectionPolicy = SerializerEndCollectionPolicy;
        using BeginCollectionItemPolicy = SerializerBeginCollectionItemPolicy;
        using EndCollectionItemPolicy = SerializerEndCollectionItemPolicy;
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
