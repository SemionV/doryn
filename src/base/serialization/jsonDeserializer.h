#pragma once

#include <utility>

#include "objectVisitor.h"
#include "base/dependencies.h"

namespace dory::typeMap::json
{
    using json = nlohmann::json;

    struct JsonContext
    {
        std::stack<json*> current;
        std::size_t dynamicCollectionIndex = 0;

        explicit JsonContext(json* data)
        {
            current.push(data);
        }
    };

    struct DeserializerValuePolicy
    {
        template<typename T>
        inline static void process(T& value, JsonContext& context)
        {
            auto* currentJson = context.current.top();
            value = currentJson->get<T>();
        }
    };

    struct DeserializerBeginMemberPolicy
    {
        inline static void process(const std::string& memberName, const std::size_t i, JsonContext& context)
        {
            auto* currentJson = context.current.top();
            auto& memberJson = currentJson->at(memberName);
            context.current.push(&memberJson);
        }
    };

    struct DeserializerEndMemberPolicy
    {
        inline static void process(const bool lastMember, JsonContext& context)
        {
            context.current.pop();
        }
    };

    struct DeserializerBeginCollectionItemPolicy
    {
        inline static void process(const std::size_t i, JsonContext& context)
        {
            auto* currentJson = context.current.top();
            auto& memberJson = currentJson->at(i);
            context.current.push(&memberJson);
        }
    };

    struct DeserializerEndCollectionItemPolicy
    {
        inline static void process(const bool lastItem, JsonContext& context)
        {
            context.current.pop();
        }
    };

    struct DeserializerDynamicCollectionPolicy
    {
        template<typename T>
        inline static std::optional<T> getNextItem(std::vector<T>& collection, JsonContext& context)
        {
            auto* currentJson = context.current.top();

            if(currentJson->is_array())
            {
                if(context.dynamicCollectionIndex < currentJson->size())
                {
                    auto& itemJson = currentJson->at(context.dynamicCollectionIndex);

                    context.current.push(&itemJson);
                    ++context.dynamicCollectionIndex;

                    if(itemJson.is_object() || itemJson.is_array())
                    {
                        return std::optional<T>{T{}};
                    }

                    return std::optional<T>{itemJson};
                }
            }

            return {};
        }

        template<typename T>
        inline static void processItem(T& item, std::vector<T>& collection, JsonContext& context)
        {
            collection.push_back(item);
            context.current.pop();
        }
    };

    struct JsonDeserializationPolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = DeserializerValuePolicy;
        using BeginMemberPolicy = DeserializerBeginMemberPolicy;
        using EndMemberPolicy = DeserializerEndMemberPolicy;
        using BeginCollectionItemPolicy = DeserializerBeginCollectionItemPolicy;
        using EndCollectionItemPolicy = DeserializerEndCollectionItemPolicy;
        using DynamicCollectionPolicyType = DeserializerDynamicCollectionPolicy;
    };

    class JsonDeserializer
    {
    public:
        template<typename T>
        static T deserialize(std::string source)
        {
            auto data = json::parse(source);
            JsonContext context(&data);
            auto object = T{};
            ObjectVisitor<JsonDeserializationPolicies>::visit(object, context);

            return object;
        }
    };
}
