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
        inline static void writeValue(T& value, ryml::NodeRef& node, YamlContext& context)
        {
            node << value;
        }

        inline static void writeValue(const std::string& value, ryml::NodeRef& node, YamlContext& context)
        {
            node = toRymlCStr(value);
        }

    public:
        template<typename T>
        inline static void process(T& value, YamlContext& context)
        {
            auto current = context.current.top();
            current |= c4::yml::NodeType_e::VAL;
            writeValue(value, current, context);
        }
    };

    struct SerializerObjectPolicy
    {
        inline static void beginObject(YamlContext& context)
        {
            auto current = context.current.top();
            current |= c4::yml::NodeType_e::MAP;
        }

        inline static void endObject(YamlContext& context)
        {
        }
    };

    struct SerializerMemberPolicy
    {
        inline static bool beginMember(const std::string_view& memberName, const std::size_t i, YamlContext& context)
        {
            auto current = context.current.top();

            auto memberNode = current.append_child();
            memberNode.set_key(toRymlCStr(memberName));
            context.current.push(memberNode);

            return true;
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
            auto current = context.current.top();
            current |= c4::yml::NodeType_e::SEQ;
#ifdef WIN32
            current |= c4::yml::NodeType_e::_WIP_STYLE_FLOW_SL;
#endif
#ifdef __unix__
            current |= c4::yml::NodeType_e::FLOW_SL;
#endif
        }

        inline static void endCollection(YamlContext& context)
        {
        }
    };

    struct SerializerCollectionItemPolicy
    {
        inline static void beginItem(const std::size_t i, YamlContext& context)
        {
            auto current = context.current.top();
            auto itemNode = current.append_child();
            context.current.push(itemNode);
        }

        inline static void endItem(const bool lastItem, YamlContext& context)
        {
            context.current.pop();
        }
    };

    struct SerializerDynamicCollectionPolicy
    {
        template<typename T>
        inline static void beginCollection(T& collection, YamlContext& context)
        {
            auto current = context.current.top();
            current |= c4::yml::NodeType_e::SEQ;

            context.previousDynamicCollectionIndex = context.dynamicCollectionIndex;
            context.dynamicCollectionIndex = 0;
        }

        template<typename T>
        inline static std::optional<std::reference_wrapper<const typename T::value_type>> nextItem(T& collection, YamlContext& context)
        {
            auto current = context.current.top();
            if(context.dynamicCollectionIndex < collection.size())
            {
                auto& item = collection[context.dynamicCollectionIndex];
                auto itemNode = current.append_child();
                context.current.push(itemNode);

                ++context.dynamicCollectionIndex;

                return {std::reference_wrapper{item}};
            }

            return {};
        }

        template<typename T>
        inline static void endItem(std::reference_wrapper<const typename T::value_type> item, T& collection, YamlContext& context)
        {
            context.current.pop();
        }

        template<typename T>
        inline static void endCollection(T& collection, YamlContext& context)
        {
            context.dynamicCollectionIndex = context.previousDynamicCollectionIndex;
            context.previousDynamicCollectionIndex = 0;
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
        static std::string serialize(const T& object)
        {
            auto tree = ryml::Tree{};

            YamlContext context(tree.rootref());
            ObjectVisitor<YamlSerializationPolicies>::visit(object, context);

            std::stringstream stream;
            stream << tree;
            return stream.str();
        }
    };
}