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
        template<class T>
        inline static bool beginMember(const std::string_view& memberName, T& value, const std::size_t i, JsonContext& context)
        {
            auto* currentJson = context.current.top();

            auto& memberJson = currentJson->operator[](memberName);
            context.current.push(&memberJson);

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
        inline static bool beginItem(const std::size_t i, JsonContext& context)
        {
            auto* currentJson = context.current.top();
            auto& itemJson = currentJson->operator[](i);
            context.current.push(&itemJson);

            return true;
        }

        inline static void endItem(const bool lastItem, JsonContext& context)
        {
            context.current.pop();
        }
    };

    struct SerializerContainerPolicy
    {
        template<typename T>
        inline static void beginCollection(T& collection, JsonContext& context)
        {
            context.collectionIndexesStack.push(0);
        }

        template<typename T>
        inline static std::optional<std::reference_wrapper<const typename T::value_type>> nextItem(T& collection, JsonContext& context)
        {
            auto* currentJson = context.current.top();
            auto& index = context.collectionIndexesStack.top();
            if(index < collection.size())
            {
                auto& item = collection[index];

                auto& itemJson = currentJson->operator[](index);
                context.current.push(&itemJson);

                ++index;

                return {std::reference_wrapper(item)};
            }

            return {};
        }

        template<typename T>
        inline static void endItem(std::reference_wrapper<const typename T::value_type> item, T& collection, JsonContext& context)
        {
            context.current.pop();
        }

        template<typename T>
        inline static void endCollection(T& collection, JsonContext& context)
        {
            context.collectionIndexesStack.pop();
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

    class JsonSerializer
    {
    public:
        template<typename T>
        static std::string serialize(const T& object, const int indent = -1)
        {
            auto data = json{};
            JsonContext context(&data);
            ObjectVisitor<JsonSerializationPolicies>::visit(object, context);

            return data.dump(indent);
        }
    };
}
