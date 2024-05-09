#pragma once

#include "objectVisitor.h"
#include "jsonSerializationContext.h"
#include "base/dependencies.h"

namespace dory::typeMap::json
{
    struct DeserializerValuePolicy
    {
        template<typename T>
        inline static void process(T& value, JsonContext& context)
        {
            auto* currentJson = context.current.top();
            if(!currentJson->empty())
            {
                value = currentJson->get<T>();
            }
        }
    };

    struct DeserializerMemberPolicy
    {
        template<typename TContext>
        inline static void beginMember(const std::string& memberName, const std::size_t i, TContext& context)
        {
            auto* currentJson = context.current.top();
            if(currentJson->contains(memberName))
            {
                auto& memberJson = currentJson->at(memberName);
                context.current.push(&memberJson);
            }
            else
            {
                context.current.push(&context.emptyJson);
            }
        }

        template<typename TContext>
        inline static void endMember(const bool lastMember, TContext& context)
        {
            context.current.pop();
        }
    };

    struct DeserializerCollectionItemPolicy
    {
        template<typename TContext>
        inline static void beginItem(const std::size_t i, TContext& context)
        {
            auto* currentJson = context.current.top();
            if(i < currentJson->size())
            {
                auto& itemJson = currentJson->at(i);
                context.current.push(&itemJson);
            }
            else
            {
                context.current.push(&context.emptyJson);
            }
        }

        template<typename TContext>
        inline static void endItem(const bool lastItem, TContext& context)
        {
            context.current.pop();
        }
    };

    struct DeserializerDynamicCollectionPolicy
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

        template<typename T>
        inline static void endCollection(std::vector<T>& collection, JsonContext& context)
        {
            context.dynamicCollectionIndex = context.previousDynamicCollectionIndex;
            context.previousDynamicCollectionIndex = 0;
        }
    };

    struct JsonDeserializationPolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = DeserializerValuePolicy;
        using MemberPolicy = DeserializerMemberPolicy;
        using CollectionItemPolicy = DeserializerCollectionItemPolicy;
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
