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
        inline static void beginMember(const std::string_view& memberName, const std::size_t i, JsonContext& context)
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

        inline static void endMember(const bool lastMember, JsonContext& context)
        {
            context.current.pop();
        }
    };

    struct DeserializerCollectionItemPolicy
    {
        inline static void beginItem(const std::size_t i, JsonContext& context)
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

        inline static void endItem(const bool lastItem, JsonContext& context)
        {
            context.current.pop();
        }
    };

    struct DeserializerDynamicCollectionPolicy
    {
        template<typename T>
        inline static void beginCollection(T& collection, JsonContext& context)
        {
            context.previousDynamicCollectionIndex = context.dynamicCollectionIndex;
            context.dynamicCollectionIndex = 0;
        }

        template<typename T>
        inline static std::optional<std::reference_wrapper<typename T::value_type>> getNextItem(T& collection, JsonContext& context)
        {
            auto* currentJson = context.current.top();

            if(currentJson->is_array())
            {
                if(context.dynamicCollectionIndex < currentJson->size())
                {
                    auto& itemJson = currentJson->at(context.dynamicCollectionIndex);

                    context.current.push(&itemJson);
                    ++context.dynamicCollectionIndex;

                    auto& item = collection.emplace_back(typename T::value_type{});

                    return {std::ref(item)};
                }
            }

            return {};
        }

        template<typename T>
        inline static void processItem(std::reference_wrapper<typename T::value_type> item,T& collection, JsonContext& context)
        {
            context.current.pop();
        }

        template<typename T>
        inline static void endCollection(T& collection, JsonContext& context)
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
        static T deserialize(const std::string& source)
        {
            auto data = json::parse(source);
            JsonContext context(&data);
            auto object = T{};
            ObjectVisitor<JsonDeserializationPolicies>::visit(object, context);

            return object;
        }
    };
}
