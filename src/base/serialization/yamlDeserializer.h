#pragma once

#include "objectVisitor.h"
#include "yamlSerializationContext.h"
#include "base/dependencies.h"

namespace dory::typeMap::yaml
{
    struct DeserializerValuePolicy
    {
    private:
        template<typename T>
        inline static void readValue(T& value, ryml::ConstNodeRef& node)
        {
            node >> value;
        }

        inline static void readValue(std::string& value, ryml::ConstNodeRef& node)
        {
            value = std::string(node.val().str, node.val().len);
        }

    public:
        template<typename T>
        inline static void process(T& value, YamlContext& context)
        {
            auto current = context.current.top();
            if(current.has_val())
            {
                readValue(value, current);
            }
        }
    };

    struct DeserializerMemberPolicy
    {
        template<typename TContext>
        inline static void beginMember(const std::string& memberName, const std::size_t i, TContext& context)
        {
            auto current = context.current.top();
            if(current.is_map() && current.has_child(memberName.data()))
            {
                auto member = current[memberName.data()];
                context.current.push(member);
            }
            else
            {
                context.current.push(current);
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
            /*auto* currentJson = context.current.top();
            if(i < currentJson->size())
            {
                auto& itemJson = currentJson->at(i);
                context.current.push(&itemJson);
            }
            else
            {
                context.current.push(&context.emptyJson);
            }*/
        }

        template<typename TContext>
        inline static void endItem(const bool lastItem, TContext& context)
        {
            //context.current.pop();
        }
    };

    struct DeserializerDynamicCollectionPolicy
    {
        template<typename T>
        inline static void beginCollection(std::vector<T>& collection, YamlContext& context)
        {
            /*context.previousDynamicCollectionIndex = context.dynamicCollectionIndex;
            context.dynamicCollectionIndex = 0;*/
        }

        template<typename T>
        inline static std::optional<T> getNextItem(std::vector<T>& collection, YamlContext& context)
        {
            /*auto* currentJson = context.current.top();

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
            }*/

            return {};
        }

        template<typename T>
        inline static void processItem(T& item, std::vector<T>& collection, YamlContext& context)
        {
            /*collection.push_back(item);
            context.current.pop();*/
        }

        template<typename T>
        inline static void endCollection(std::vector<T>& collection, YamlContext& context)
        {
            /*context.dynamicCollectionIndex = context.previousDynamicCollectionIndex;
            context.previousDynamicCollectionIndex = 0;*/
        }
    };

    struct YamlDeserializationPolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = DeserializerValuePolicy;
        using MemberPolicy = DeserializerMemberPolicy;
        using CollectionItemPolicy = DeserializerCollectionItemPolicy;
        using DynamicCollectionPolicyType = DeserializerDynamicCollectionPolicy;
    };

    class YamlDeserializer
    {
    public:
        template<typename T>
        static T deserialize(std::string source, T& object)
        {
            auto tree = ryml::parse_in_place(source.data());
            YamlContext context(tree.rootref());
            ObjectVisitor<YamlDeserializationPolicies>::visit(object, context);

            return object;
        }
    };
}