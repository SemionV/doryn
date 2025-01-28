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

    struct DeserializerEnumPolicy
    {
        template<typename T>
        static void process(T& value, JsonContext& context)
        {
            if(const auto current = context.parents.top(); current && !current->empty())
            {
                if(auto enumValue = magic_enum::enum_cast<T>(current->get<std::string_view>()))
                {
                    value = *enumValue;
                }
            }
        }
    };

    struct DeserializerMemberPolicy
    {
        static bool beginMember(auto&& member, const std::size_t i, JsonContext& context)
        {
            auto* currentJson = context.parents.top();
            if(currentJson->contains(member.name))
            {
                auto& memberJson = currentJson->at(member.name);
                context.parents.push(&memberJson);

                return true;
            }

            return false;
        }

        template<class T, class TValue>
        static bool beginMember(reflection::ClassMember<T, std::optional<TValue>>& member, const std::size_t i, JsonContext& context)
        {
            if(beginMember(member, i, context))
            {
                member.value.emplace();
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
        requires(generic::is_dynamic_collection_v<TCollection>)
        static void setCollectionSize(TCollection& collection, std::stack<NodeType>& parents, std::size_t& size)
        {
            auto* currentJson = parents.top();
            size = currentJson->is_array() ? currentJson->size() : 0;
        }

        template<typename TCollection>
        requires(generic::is_dictionary_v<TCollection>)
        static void setCollectionSize(TCollection& collection, std::stack<NodeType>& parents, std::size_t& size)
        {
            auto* currentJson = parents.top();
            if(currentJson->is_object())
            {
                size = std::distance(currentJson->items().begin(), currentJson->items().end());
            }
            else
            {
                size = 0;
            }
        }

        template<typename TCollection>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static auto& getCollectionItem(TCollection& collection, auto& index, std::stack<NodeType>& parents)
        {
            auto* currentJson = parents.top();
            auto& itemJson = currentJson->at(index);
            parents.push(&itemJson);

            return collection.emplace_back(typename TCollection::value_type{});
        }

        template<typename TCollection>
        requires(generic::is_dictionary_v<TCollection>)
        static auto& getCollectionItem(TCollection& collection, auto& index, std::stack<NodeType>& parents)
        {
            using ValueType = typename TCollection::mapped_type;

            if(auto* currentJson = parents.top(); currentJson->is_object())
            {
                std::size_t currentIndex = 0;
                for(auto& iter : currentJson->items())
                {
                    if(currentIndex == index)
                    {
                        const auto stringKey = iter.key();
                        const auto key = getKeyValue<TCollection>(stringKey);
                        auto& itemJson = currentJson->at(stringKey);
                        parents.push(&itemJson);

                        auto it = collection.find(key);
                        if(it != collection.end())
                        {
                            return it->second;
                        }

                        if(auto result = collection.emplace(key, ValueType{}); result.second)
                        {
                            return result.first->second;
                        }

                        break;
                    }

                    currentIndex++;
                }
            }

            return collection[typename TCollection::key_type{}]; //in case if normal emplace to the dictionary did not work, we return an item for default value of the key
        }
    };

    struct JsonDeserializationPolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = DeserializerValuePolicy;
        using EnumPolicy = DeserializerEnumPolicy;
        using MemberPolicy = DeserializerMemberPolicy;
        using CollectionItemPolicy = DeserializerCollectionItemPolicy;
        using ContainerPolicyType = DeserializerContainerPolicy;
    };

    template<typename T, typename TVisitorBase = ObjectVisitorDefaultBase>
    static T deserialize(const std::string& source)
    {
        auto data = json::parse(source);
        JsonContext context(&data);
        auto object = T{};
        ObjectVisitor<JsonDeserializationPolicies, TVisitorBase>::visit(object, context);

        return object;
    }

    template<typename T, typename TVisitorBase = ObjectVisitorDefaultBase>
    static void deserialize(const std::string& source, T& object)
    {
        auto data = json::parse(source);
        JsonContext context(&data);
        ObjectVisitor<JsonDeserializationPolicies, TVisitorBase>::visit(object, context);
    }
}
