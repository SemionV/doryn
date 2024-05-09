#pragma once

#include "objectVisitor.h"
#include "yamlSerializationContext.h"
#include "base/dependencies.h"

namespace dory::typeMap::yaml
{
    struct SerializerValuePolicy
    {
    private:
        template<typename T>
        inline static void writeValue(T& value, ryml::NodeRef& node)
        {
            node << value;
        }

        inline static void writeValue(std::string& value, ryml::NodeRef& node)
        {
            node = value.data();
        }

    public:
        template<typename T>
        inline static void process(T& value, YamlContext& context)
        {
            auto current = context.current.top();
            current |= c4::yml::NodeType_e::VAL;
            //current.set_type(c4::yml::NodeType{c4::yml::NodeType_e::VAL});
            writeValue(value, current);
        }
    };

    struct SerializerObjectPolicy
    {
        inline static void beginObject(YamlContext& context)
        {
            auto current = context.current.top();
            current |= c4::yml::NodeType_e::MAP;
            //current.set_type(c4::yml::NodeType{c4::yml::NodeType_e::MAP});

            /*auto* currentJson = context.current.top();
            *currentJson = json::object();*/
        }

        inline static void endObject(YamlContext& context)
        {
        }
    };

    struct SerializerMemberPolicy
    {
        inline static void beginMember(const std::string& memberName, const std::size_t i, YamlContext& context)
        {
            auto currentNode = context.current.top();
            auto memberNode = currentNode[memberName.data()];
            context.current.push(memberNode);
        }

        inline static void endMember(const bool lastMember, YamlContext& context)
        {
            context.current.pop();
        }
    };

    struct SerializerCollectionPolicy
    {
        template<typename T, auto N>
        inline static void beginCollection(YamlContext& context)
        {
            /*auto* currentJson = context.current.top();
            *currentJson = json::array();*/
        }

        inline static void endCollection(YamlContext& context)
        {
        }
    };

    struct SerializerCollectionItemPolicy
    {
        inline static void beginItem(const std::size_t i, YamlContext& context)
        {
            /*auto* currentJson = context.current.top();
            auto& itemJson = currentJson->operator[](i);
            context.current.push(&itemJson);*/
        }

        inline static void endItem(const bool lastItem, YamlContext& context)
        {
            /*context.current.pop();*/
        }
    };

    struct SerializerDynamicCollectionPolicy
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
            if(context.dynamicCollectionIndex < collection.size())
            {
                auto& item = collection[context.dynamicCollectionIndex];

                auto& itemJson = currentJson->operator[](context.dynamicCollectionIndex);
                context.current.push(&itemJson);

                ++context.dynamicCollectionIndex;

                return item;
            }

            return {};*/
        }

        template<typename T>
        inline static void processItem(T& item, std::vector<T>& collection, YamlContext& context)
        {
            /*context.current.pop();*/
        }

        template<typename T>
        inline static void endCollection(std::vector<T>& collection, YamlContext& context)
        {
            /*context.dynamicCollectionIndex = context.previousDynamicCollectionIndex;
            context.previousDynamicCollectionIndex = 0;*/
        }
    };

    struct YamlSerializationPolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = SerializerValuePolicy;
        using ObjectPolicy = SerializerObjectPolicy;
        using MemberPolicy = SerializerMemberPolicy;
        using CollectionPolicy = SerializerCollectionPolicy;
        using CollectionItemPolicy = SerializerCollectionItemPolicy;
        using DynamicCollectionPolicyType = SerializerDynamicCollectionPolicy;
    };

    class YamlSerializer
    {
    public:
        template<typename T>
        static std::string serialize(T& object)
        {
            auto tree = ryml::Tree{};

            YamlContext context(tree.rootref());
            ObjectVisitor<YamlSerializationPolicies>::visit(object, context);

            std::stringstream ss;
            ss << tree;
            std::string stream_result = ss.str();

            return stream_result;
            //return ryml::emitrs_yaml<std::string>(tree);
        }
    };
}