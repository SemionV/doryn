#pragma once

#include <utility>

#include "objectVisitor.h"
#include "base/dependencies.h"

namespace dory::typeMap::json
{
    using json = nlohmann::json;

    struct JsonSerializationContext
    {
        std::stack<json*> current;
        json emptyJson = {};
        std::size_t dynamicCollectionIndex = 0;
        std::size_t previousDynamicCollectionIndex = 0;

        explicit JsonSerializationContext(json* data)
        {
            current.push(data);
        }
    };

    struct SerializerValuePolicy
    {
        template<typename T>
        inline static void process(const T& value, JsonSerializationContext& context)
        {
            auto* currentJson = context.current.top();
            *currentJson = value;
        }
    };

    struct SerializerBeginObjectPolicy
    {
        inline static void process(JsonSerializationContext& context)
        {
            auto* currentJson = context.current.top();
            *currentJson = json::object();
        }
    };

    struct SerializerEndObjectPolicy
    {
        inline static void process(JsonSerializationContext& context)
        {
        }
    };

    struct SerializerBeginMemberPolicy
    {
        inline static void process(const std::string& memberName, const std::size_t i, JsonSerializationContext& context)
        {
            auto* currentJson = context.current.top();

            auto& memberJson = currentJson->operator[](memberName);
            context.current.push(&memberJson);
        }
    };

    struct SerializerEndMemberPolicy
    {
        inline static void process(const bool lastMember, JsonSerializationContext& context)
        {
            context.current.pop();
        }
    };

    struct SerializerBeginCollectionPolicy
    {
        template<typename T, auto N>
        inline static void process(JsonSerializationContext& context)
        {
            auto* currentJson = context.current.top();
            *currentJson = json::array();
        }
    };

    struct SerializerEndCollectionPolicy
    {
        inline static void process(JsonSerializationContext& context)
        {
        }
    };

    struct SerializerBeginCollectionItemPolicy
    {
        inline static void process(const std::size_t i, JsonSerializationContext& context)
        {
            auto* currentJson = context.current.top();
            auto& itemJson = currentJson->operator[](i);
            context.current.push(&itemJson);
        }
    };

    struct SerializerEndCollectionItemPolicy
    {
        inline static void process(const bool lastItem, JsonSerializationContext& context)
        {
            context.current.pop();
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
        /*using DynamicCollectionPolicyType = DeserializerDynamicCollectionPolicy;*/
    };

    class JsonSerializer
    {
    public:
        template<typename T>
        static std::string serialize(T& object, const int indent = -1)
        {
            auto data = json{};
            JsonSerializationContext context(&data);
            ObjectVisitor<JsonSerializationPolicies>::visit(object, context);

            return data.dump(indent);
        }
    };
}
