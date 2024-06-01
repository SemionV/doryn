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
        template<class T>
        inline static bool beginMember(const std::string_view& memberName, T& value, const std::size_t i, YamlContext& context)
        {
            auto current = context.current.top();

            auto memberNode = current.append_child();
            memberNode.set_key(toRymlCStr(memberName));
            context.current.push(memberNode);

            return true;
        }

        template<class T>
        inline static bool beginMember(const std::string_view& memberName, const std::optional<T>& value, const std::size_t i, YamlContext& context)
        {
            if(value.has_value())
            {
                return beginMember(memberName, *value, i, context);
            }

            return false;
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
        inline static bool beginItem(const std::size_t i, YamlContext& context)
        {
            auto current = context.current.top();
            auto itemNode = current.append_child();
            context.current.push(itemNode);

            return true;
        }

        inline static void endItem(const bool lastItem, YamlContext& context)
        {
            context.current.pop();
        }
    };

    struct SerializerContainerPolicy
    {
        template<typename TCollection>
        requires(is_dynamic_collection_v<TCollection>)
        inline static void beginCollection(TCollection& collection, YamlContext& context)
        {
            auto current = context.current.top();
            current |= c4::yml::NodeType_e::SEQ;
            context.collectionIndexesStack.push(0);
        }
        template<typename TCollection>
        requires(is_dictionary_v<TCollection>)
        inline static void beginCollection(TCollection& collection, YamlContext& context)
        {
            auto current = context.current.top();
            current |= c4::yml::NodeType_e::MAP;

            auto& keys = context.dictionaryKeysStack.emplace();
            for(auto& pair : collection)
            {
                keys.emplace(pair.first);
            }
        }

        template<typename T>
        inline static auto nextItem(T& collection, YamlContext& context)
        {
            auto current = context.current.top();
            if(itemsLeft(collection, context))
            {
                auto& item = getItem(collection, context, current);
                return std::optional{std::ref(item)};
            }

            using ValueType = decltype(getItem(std::declval<T&>(), std::declval<YamlContext&>(), std::declval<ryml::NodeRef&>()));
            return std::optional<decltype(std::ref(std::declval<ValueType>()))>{};
        }

        template<typename TCollection>
        requires(is_dynamic_collection_v<TCollection>)
        inline static bool itemsLeft(TCollection& collection, YamlContext& context)
        {
            return context.collectionIndexesStack.top() < collection.size();
        }

        template<typename TCollection>
        requires(is_dynamic_collection_v<TCollection>)
        inline static auto& getItem(TCollection& collection, YamlContext& context, ryml::NodeRef& containerNode)
        {
            auto& index = context.collectionIndexesStack.top();
            auto& item = collection[index];
            ++index;

            auto itemNode = containerNode.append_child();
            context.current.push(itemNode);

            return item;
        }

        template<typename TCollection>
        requires(is_dictionary_v<TCollection>)
        inline static bool itemsLeft(TCollection& collection, YamlContext& context)
        {
            auto& keys = context.dictionaryKeysStack.top();
            return !keys.empty() && collection.contains(keys.front());
        }

        template<typename TCollection>
        requires(is_dictionary_v<TCollection>)
        inline static auto& getItem(TCollection& collection, YamlContext& context, ryml::NodeRef& containerNode)
        {
            auto& keys = context.dictionaryKeysStack.top();
            auto& key = keys.front();
            keys.pop();

            auto itemNode = containerNode.append_child();
            itemNode.set_key(toRymlCStr(key));
            context.current.push(itemNode);

            return collection.at(key);
        }

        template<typename T>
        inline static void endItem(auto& item, T& collection, YamlContext& context)
        {
            context.current.pop();
        }

        template<typename TCollection>
        requires(is_dynamic_collection_v<TCollection>)
        inline static void endCollection(TCollection& collection, YamlContext& context)
        {
            context.collectionIndexesStack.pop();
        }
        template<typename TCollection>
        requires(is_dictionary_v<TCollection>)
        inline static void endCollection(TCollection& collection, YamlContext& context)
        {
            context.dictionaryKeysStack.pop();
        }
    };

    struct YamlSerializationPolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = SerializerValuePolicy;
        using ObjectPolicy = SerializerObjectPolicy;
        using MemberPolicy = SerializerMemberPolicy;
        using CollectionPolicy = SerializerCollectionPolicy;
        using CollectionItemPolicy = SerializerCollectionItemPolicy;
        using ContainerPolicyType = SerializerContainerPolicy;
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