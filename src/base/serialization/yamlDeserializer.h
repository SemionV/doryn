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
        template<class T>
        inline static bool beginMember(const std::string_view& memberName, const T& value, const std::size_t i, YamlContext& context)
        {
            auto current = context.current.top();
            const auto& name = toRymlCStr(memberName);
            if(current.is_map() && current.has_child(name))
            {
                auto member = current[name];
                context.current.push(member);

                return true;
            }

            return false;
        }

        template<class T>
        inline static bool beginMember(const std::string_view& memberName, std::optional<T>& value, const std::size_t i, YamlContext& context)
        {
            if(beginMember(memberName, *value, i, context))
            {
                value = T{};
                return true;
            }

            return false;
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

    struct DeserializerContainerPolicy
    {
        template<typename T>
        inline static void beginCollection(T& collection, YamlContext& context)
        {
            context.collectionIndexesStack.push(0);
        }

        template<typename T>
        inline static auto nextItem(T& collection, YamlContext& context)
        {
            auto current = context.current.top();
            auto& index = context.collectionIndexesStack.top();
            if(index < current.num_children() && (current.is_seq() || current.is_map()))
            {
                auto itemNode = current.at(index);

                context.current.push(itemNode);
                ++index;

                auto& item = insertItem(collection, itemNode);
                return std::optional{std::ref(item)};
            }

            return std::optional<std::reference_wrapper<CollectionValueTypeType<T>>>{};
        }

        template<typename TCollection>
        requires(is_dynamic_collection_v<TCollection>)
        inline static auto& insertItem(TCollection& collection, const ryml::NodeRef& node)
        {
            return collection.emplace_back(typename TCollection::value_type{});
        }

        template<typename TCollection>
        requires(is_dictionary_v<TCollection>)
        inline static auto& insertItem(TCollection& collection, const ryml::NodeRef& node)
        {
            auto nodeKey = node.key();
            auto key = std::string(nodeKey.str, nodeKey.len);
            if(collection.contains(key))
            {
                return collection[key];
            }

            return collection[key] = typename TCollection::mapped_type{};
        }

        template<typename T>
        inline static void endItem(auto& item, T& collection, YamlContext& context)
        {
            context.current.pop();
        }

        template<typename T>
        inline static void endCollection(T& collection, YamlContext& context)
        {
            context.collectionIndexesStack.pop();
        }
    };

    struct YamlDeserializationPolicies: public VisitorDefaultPolicies
    {
        using ValuePolicy = DeserializerValuePolicy;
        using MemberPolicy = DeserializerMemberPolicy;
        using CollectionItemPolicy = DeserializerCollectionItemPolicy;
        using ContainerPolicyType = DeserializerContainerPolicy;
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