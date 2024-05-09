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
        inline static void readValue(T& value, ryml::NodeRef& node)
        {
            node >> value;
        }

        inline static void readValue(std::string& value, ryml::NodeRef& node)
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
        inline static void beginMember(const std::string& memberName, const std::size_t i, YamlContext& context)
        {
            auto current = context.current.top();
            const auto& name = toRymlCStr(memberName);
            if(current.is_map() && current.has_child(name))
            {
                auto member = current[name];
                context.current.push(member);
            }
            else
            {
                context.current.push(current);
            }
        }

        inline static void endMember(const bool lastMember, YamlContext& context)
        {
            context.current.pop();
        }
    };

    struct DeserializerCollectionItemPolicy
    {
        inline static void beginItem(const std::size_t i, YamlContext& context)
        {
            auto current = context.current.top();
            if(current.is_seq())
            {
                auto itemNode = current.at(i);
                context.current.push(itemNode);
            }
        }

        inline static void endItem(const bool lastItem, YamlContext& context)
        {
            context.current.pop();
        }
    };

    struct DeserializerDynamicCollectionPolicy
    {
        template<typename T>
        inline static void beginCollection(std::vector<T>& collection, YamlContext& context)
        {
            context.previousDynamicCollectionIndex = context.dynamicCollectionIndex;
            context.dynamicCollectionIndex = 0;
        }

        template<typename T>
        inline static std::optional<T> getNextItem(std::vector<T>& collection, YamlContext& context)
        {
            auto current = context.current.top();
            if(current.is_seq() && context.dynamicCollectionIndex < current.num_children())
            {
                auto itemNode = current.at(context.dynamicCollectionIndex);
                context.current.push(itemNode);
                ++context.dynamicCollectionIndex;

                return T{};
            }

            return {};
        }

        template<typename T>
        inline static void processItem(T& item, std::vector<T>& collection, YamlContext& context)
        {
            collection.push_back(item);
            context.current.pop();
        }

        template<typename T>
        inline static void endCollection(std::vector<T>& collection, YamlContext& context)
        {
            context.dynamicCollectionIndex = context.previousDynamicCollectionIndex;
            context.previousDynamicCollectionIndex = 0;
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
            auto tree = ryml::parse_in_place(toRymlStr(source));
            YamlContext context(tree.rootref());
            ObjectVisitor<YamlDeserializationPolicies>::visit(object, context);

            return object;
        }

        template<typename T>
        static T deserialize(std::string source)
        {
            auto object = T{};

            deserialize(source, object);

            return object;
        }
    };
}