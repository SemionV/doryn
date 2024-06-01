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

    struct DeserializerContainerPolicy: public ContainerPolicy<DeserializerContainerPolicy, TreeStructureContext<json*>>
    {
        using NodeType = json*;

        template<typename TCollection>
        inline static void setCollectionSize(TCollection& collection, ContextType& context, std::size_t& size)
        {
            auto* currentJson = context.current.top();
            size = currentJson->is_array() ? currentJson->size() : 0;
        }

        template<typename TCollection, typename TKeysContainer>
        inline static void buildDictionaryKeysList(TCollection& collection, ContextType& context, TKeysContainer& keys)
        {
            auto* currentJson = context.current.top();
            if(currentJson->is_object())
            {
                for(auto& iter : currentJson->items())
                {
                    keys.emplace(iter.key());
                }
            }
        }

        template<typename TCollection>
        inline static auto& getCollectionItem(TCollection& collection, auto& index, std::stack<NodeType>& parents)
        {
            auto* currentJson = parents.top();
            auto& itemJson = currentJson->at(index);
            parents.push(&itemJson);

            return collection.emplace_back(typename TCollection::value_type{});
        }

        template<typename TCollection>
        inline static auto& getDictionaryItem(TCollection& collection, const auto& key, std::stack<NodeType>& parents)
        {
            auto* currentJson = parents.top();
            auto& itemJson = currentJson->at(key);
            parents.push(&itemJson);

            if(collection.contains(key))
            {
                return collection[key];
            }

            return collection[key] = typename TCollection::mapped_type{};
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
