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
        template<class T>
        inline static bool beginMember(const std::string_view& memberName, T& value, const std::size_t i, JsonContext& context)
        {
            auto* currentJson = context.current.top();
            if(currentJson->contains(memberName))
            {
                auto& memberJson = currentJson->at(memberName);
                context.current.push(&memberJson);

                return true;
            }

            return false;
        }

        template<class T>
        inline static bool beginMember(const std::string_view& memberName, std::optional<T>& value, const std::size_t i, JsonContext& context)
        {
            if(beginMember(memberName, *value, i, context))
            {
                value = T{};
                return true;
            }

            return false;
        }

        inline static void endMember(const bool lastMember, JsonContext& context)
        {
            context.current.pop();
        }
    };

    struct DeserializerCollectionItemPolicy
    {
        inline static bool beginItem(const std::size_t i, JsonContext& context)
        {
            auto* currentJson = context.current.top();
            if(i < currentJson->size())
            {
                auto& itemJson = currentJson->at(i);
                context.current.push(&itemJson);

                return true;
            }

            return false;
        }

        inline static void endItem(const bool lastItem, JsonContext& context)
        {
            context.current.pop();
        }
    };

    struct DeserializerContainerPolicy
    {
        template<typename T>
        inline static void beginCollection(T& collection, JsonContext& context)
        {
            context.collectionIndexesStack.push(0);
        }

        template<typename T>
        inline static std::optional<std::reference_wrapper<typename T::value_type>> nextItem(T& collection, JsonContext& context)
        {
            auto* currentJson = context.current.top();

            if(currentJson->is_array())
            {
                auto& index = context.collectionIndexesStack.top();
                if(index < currentJson->size())
                {
                    auto& itemJson = currentJson->at(index);

                    context.current.push(&itemJson);
                    ++index;

                    auto& item = collection.emplace_back(typename T::value_type{});

                    return {std::ref(item)};
                }
            }
            /*else if(currentJson->is_object())
            {
                auto& index = context.collectionIndexesStack.top();
                if(index < currentJson->size())
                {
                    auto& itemJson = currentJson->at(index);

                    context.current.push(&itemJson);
                    ++index;

                    auto nodeKey = itemJson.items().begin().value()
                }
            }*/

            return {};
        }

        template<typename T>
        inline static void endItem(std::reference_wrapper<typename T::value_type> item,T& collection, JsonContext& context)
        {
            context.current.pop();
        }

        template<typename T>
        inline static void endCollection(T& collection, JsonContext& context)
        {
            context.collectionIndexesStack.pop();
        }
    };

    struct JsonDeserializationPolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = DeserializerValuePolicy;
        using MemberPolicy = DeserializerMemberPolicy;
        using CollectionItemPolicy = DeserializerCollectionItemPolicy;
        using ContainerPolicyType = DeserializerContainerPolicy;
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
