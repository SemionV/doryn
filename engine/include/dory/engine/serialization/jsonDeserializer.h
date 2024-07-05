#pragma once

#include "objectVisitor.h"
#include "jsonSerializationContext.h"

namespace dory::serialization::json
{
    struct DeserializerValuePolicy
    {
        template<typename T>
        inline static void process(T& value, JsonContext& context)
        {
            auto* currentJson = context.parents.top();
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
            auto* currentJson = context.parents.top();
            if(currentJson->contains(memberName))
            {
                auto& memberJson = currentJson->at(memberName);
                context.parents.push(&memberJson);

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
            context.parents.pop();
        }
    };

    struct DeserializerCollectionItemPolicy
    {
        inline static bool beginItem(const std::size_t i, JsonContext& context)
        {
            auto* currentJson = context.parents.top();
            if(i < currentJson->size())
            {
                auto& itemJson = currentJson->at(i);
                context.parents.push(&itemJson);

                return true;
            }

            return false;
        }

        inline static void endItem(const bool lastItem, JsonContext& context)
        {
            context.parents.pop();
        }
    };

    struct DeserializerContainerPolicy: public ContainerPolicy<DeserializerContainerPolicy, TreeStructureContext<json*>>
    {
        template<typename TCollection>
        inline static void setCollectionSize(TCollection& collection, std::stack<NodeType>& parents, std::size_t& size)
        {
            auto* currentJson = parents.top();
            size = currentJson->is_array() ? currentJson->size() : 0;
        }

        template<typename TCollection, typename TKeysContainer>
        inline static void buildDictionaryKeysList(TCollection& collection, std::stack<NodeType>& parents, TKeysContainer& keys)
        {
            auto* currentJson = parents.top();
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

            const std::string keyString { key };
            if(collection.contains(keyString))
            {
                return collection[keyString];
            }

            return collection[keyString] = typename TCollection::mapped_type{};
        }
    };

    struct JsonDeserializationPolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = DeserializerValuePolicy;
        using MemberPolicy = DeserializerMemberPolicy;
        using CollectionItemPolicy = DeserializerCollectionItemPolicy;
        using ContainerPolicyType = DeserializerContainerPolicy;
    };

    template<typename T>
    static T deserialize(const std::string& source)
    {
        auto data = json::parse(source);
        JsonContext context(&data);
        auto object = T{};
        ObjectVisitor<JsonDeserializationPolicies>::visit(object, context);

        return object;
    }

    template<typename T>
    static T deserialize(const std::string& source, T& object)
    {
        auto data = json::parse(source);
        JsonContext context(&data);
        ObjectVisitor<JsonDeserializationPolicies>::visit(object, context);

        return object;
    }
}
