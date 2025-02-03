#pragma once

#include "objectVisitor.h"
#include "jsonSerializationContext.h"

namespace dory::serialization::json
{
    struct DeserializerValuePolicy
    {
        template<typename T, typename TRegistry, typename TDataContext>
        static void process(T& value, JsonContext<TRegistry, TDataContext>& context)
        {
            if(auto* currentJson = context.node; !currentJson->empty())
            {
                value = currentJson->template get<T>();
            }
        }
    };

    struct DeserializerEnumPolicy
    {
        template<typename T, typename TRegistry, typename TDataContext>
        static void process(T& value, JsonContext<TRegistry, TDataContext>& context)
        {
            if(const auto current = context.node; current && !current->empty())
            {
                if(auto enumValue = magic_enum::enum_cast<T>(current->template get<std::string_view>()))
                {
                    value = *enumValue;
                }
            }
        }
    };

    struct DeserializerMemberPolicy
    {
        template<typename TRegistry, typename TDataContext>
        static std::optional<JsonContext<TRegistry, TDataContext>> beginMember(auto&& member, const std::size_t i, JsonContext<TRegistry, TDataContext>& context)
        {
            if(auto* currentJson = context.node; currentJson->contains(member.name))
            {
                auto& memberJson = currentJson->at(member.name);
                return JsonContext{ &memberJson, context.registry, context.dataContext };
            }

            return {};
        }

        template<class T, class TValue, typename TRegistry, typename TDataContext>
        static std::optional<JsonContext<TRegistry, TDataContext>> beginMember(reflection::ClassMember<T, std::optional<TValue>>& member, const std::size_t i, JsonContext<TRegistry, TDataContext>& context)
        {
            if(auto memberContext = beginMember(member, i, context))
            {
                member.value.emplace();
                return memberContext;
            }

            return {};
        }

        template<typename TRegistry, typename TDataContext>
        static void endMember(const bool lastMember, JsonContext<TRegistry, TDataContext>& context)
        {}
    };

    struct DeserializerCollectionItemPolicy
    {
        template<typename TRegistry, typename TDataContext>
        static std::optional<JsonContext<TRegistry, TDataContext>> beginItem(const std::size_t i, JsonContext<TRegistry, TDataContext>& context)
        {
            if(auto* currentJson = context.node; i < currentJson->size())
            {
                auto& itemJson = currentJson->at(i);
                return JsonContext{ &itemJson, context.registry, context.dataContext };
            }

            return {};
        }

        template<typename TRegistry, typename TDataContext>
        static void endItem(const bool lastItem, JsonContext<TRegistry, TDataContext>& context)
        {}
    };

    struct DeserializerContainerPolicy: public ContainerPolicy<DeserializerContainerPolicy>
    {
        using NodeType = json*;

        template<typename TCollection>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static void setCollectionSize(TCollection& collection, NodeType& collectionNode, std::size_t& size)
        {
            size = collectionNode->is_array() ? collectionNode->size() : 0;
        }

        template<typename TCollection>
        requires(generic::is_dictionary_v<TCollection>)
        static void setCollectionSize(TCollection& collection, NodeType& collectionNode, std::size_t& size)
        {
            if(collectionNode->is_object())
            {
                size = std::distance(collectionNode->items().begin(), collectionNode->items().end());
            }
            else
            {
                size = 0;
            }
        }

        template<typename TCollection, typename TItem, typename TRegistry, typename TDataContext>
        requires(generic::is_dynamic_collection_v<TCollection>)
        static std::optional<JsonContext<TRegistry, TDataContext>> getCollectionItem(JsonContext<TRegistry, TDataContext>& context,
            TCollection& collection, auto& index, NodeType& collectionNode, TItem** item)
        {
            auto& itemJson = collectionNode->at(index);
            *item = &collection.emplace_back(typename TCollection::value_type{});
            return JsonContext{ &itemJson, context.registry, context.dataContext };
        }

        template<typename TCollection, typename TItem, typename TRegistry, typename TDataContext>
        requires(generic::is_dictionary_v<TCollection>)
        static std::optional<JsonContext<TRegistry, TDataContext>> getCollectionItem(JsonContext<TRegistry, TDataContext>& context,
            TCollection& collection, auto& index, NodeType& collectionNode, TItem** item)
        {
            if(collectionNode->is_object())
            {
                std::size_t currentIndex = 0;
                for(auto& iter : collectionNode->items())
                {
                    if(currentIndex == index)
                    {
                        const auto& stringKey = iter.key();
                        const auto key = getKeyValue<TCollection>(stringKey);
                        auto& itemJson = collectionNode->at(stringKey);

                        auto it = collection.find(key);
                        if(it != collection.end())
                        {
                            *item = &it->second;
                        }
                        else if(auto result = collection.emplace(key, TItem{}); result.second)
                        {
                            *item = &result.first->second;
                        }
                        else
                        {
                            //in case if normal emplace to the dictionary did not work, we return an item for default value of the key
                            *item = &collection.emplace(typename TCollection::key_type{}, TItem{}).first->second;
                        }

                        return JsonContext{ &itemJson, context.registry, context.dataContext };
                    }

                    currentIndex++;
                }
            }

            return {};
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

    template<typename T, typename TRegistry, typename TDataContext, typename... TVisitorBases>
    static T deserialize(const std::string& source, TRegistry& registry, TDataContext& dataContext)
    {
        auto data = json::parse(source);
        JsonContext<TRegistry, TDataContext> context(&data, registry, dataContext);
        auto object = T{};
        ObjectVisitor<JsonDeserializationPolicies, TVisitorBases...>::visit(object, context);

        return object;
    }

    template<typename T, typename TRegistry, typename TDataContext, typename... TVisitorBases>
    static void deserialize(const std::string& source, T& object, TRegistry& registry, TDataContext& dataContext)
    {
        auto data = json::parse(source);
        JsonContext<TRegistry, TDataContext> context(&data, registry, dataContext);
        ObjectVisitor<JsonDeserializationPolicies, TVisitorBases...>::visit(object, context);
    }
}
