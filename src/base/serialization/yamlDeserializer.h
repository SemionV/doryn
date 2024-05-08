#pragma once

#include "objectVisitor.h"
#include "yamlSerializationContext.h"
#include "base/dependencies.h"

namespace dory::typeMap::yaml
{
    struct DeserializerValuePolicy
    {
        template<typename T>
        inline static void process(T& value, YamlContext& context)
        {
            auto current = context.current.top();
            //if(current.is_val() && !current.empty())
            {
                current >> value;
            }
        }

        inline static void process(std::string& value, YamlContext& context)
        {
            auto current = context.current.top();
            //if(current.is_val() && !current.empty())
            {
                value = std::string(current.val().str, current.val().len);
            }
        }
    };

    struct DeserializerBeginMemberPolicy
    {
        inline static void process(const std::string& memberName, const std::size_t i, YamlContext& context)
        {
            auto current = context.current.top();
            if(current.is_map())
            {
                auto member = current[memberName.data()];
                if(member.key_is_null())
                {
                    context.current.push(ryml::NodeRef());
                }
                else
                {
                    context.current.push(member);
                }
            }
        }
    };

    struct DeserializerEndMemberPolicy
    {
        inline static void process(const bool lastMember, YamlContext& context)
        {
            context.current.pop();
        }
    };

    struct DeserializerBeginCollectionItemPolicy
    {
        inline static void process(const std::size_t i, YamlContext& context)
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
    };

    struct DeserializerEndCollectionItemPolicy
    {
        inline static void process(const bool lastItem, YamlContext& context)
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
        using BeginMemberPolicy = DeserializerBeginMemberPolicy;
        using EndMemberPolicy = DeserializerEndMemberPolicy;
        using BeginCollectionItemPolicy = DeserializerBeginCollectionItemPolicy;
        using EndCollectionItemPolicy = DeserializerEndCollectionItemPolicy;
        using DynamicCollectionPolicyType = DeserializerDynamicCollectionPolicy;
    };

    class YamlDeserializer
    {
    public:
        template<typename T>
        static T deserialize(std::string source)
        {
            auto tree = ryml::parse_in_place(source.data());
            YamlContext context(tree.rootref());
            auto name = tree.rootref()["name"].val();
            auto object = T{};
            ObjectVisitor<YamlDeserializationPolicies>::visit(object, context);

            return object;
        }
    };
}