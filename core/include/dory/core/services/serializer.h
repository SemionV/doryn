#pragma once

#include <dory/core/services/iSerializer.h>
#include <dory/core/implementation.h>
#include <dory/serialization/yamlSerializer.h>
#include <dory/serialization/yamlDeserializer.h>
#include <dory/serialization/jsonSerializer.h>
#include <dory/serialization/jsonDeserializer.h>

namespace dory::core::services::serialization
{
    //GLM extensions to the base ObjectVisitor
    template<typename TPolicies>
    struct GlmObjectVisitor
    {
        using PoliciesType = TPolicies;
        using VisitorType = dory::serialization::ObjectVisitor<PoliciesType, GlmObjectVisitor>;

        template<typename T, auto size, typename TContext>
        static void visit(glm::vec<size, T>& vector, TContext& context)
        {
            dory::serialization::ArrayVisitor<T, size, PoliciesType, VisitorType>::visit(vector, context);
        }

        template<typename T, auto size, typename TContext>
        static void visit(const glm::vec<size, T>& vector, TContext& context)
        {
            dory::serialization::ArrayVisitor<const T, size, PoliciesType, VisitorType>::visit(vector, context);
        }

        constexpr static std::size_t quaternionSize = 4;

        template<typename T, typename TContext>
        static void visit(glm::qua<T>& vector, TContext& context)
        {
            dory::serialization::ArrayVisitor<T, quaternionSize, PoliciesType, VisitorType>::visit(vector, context);
        }

        template<typename T, typename TContext>
        static void visit(const glm::qua<T>& vector, TContext& context)
        {
            dory::serialization::ArrayVisitor<const T, quaternionSize, PoliciesType, VisitorType>::visit(vector, context);
        }
    };

    //Localization extensions to the base ObjectVisitor
    template<typename TPolicies>
    struct ParameterizedStringObjectVisitor
    {
        using PoliciesType = TPolicies;
        using VisitorType = dory::serialization::ObjectVisitor<PoliciesType, ParameterizedStringObjectVisitor>;

        template<typename T, typename TContext>
        requires(std::is_base_of_v<generic::IParameterizedString, std::decay_t<T>>)
        static void visit(T&& object, TContext& context)
        {
            TPolicies::ValuePolicy::process(object.getTemplate(), context);
            object.updateTemplate();
        }

        template<typename T, typename TContext>
        requires(std::is_base_of_v<generic::IParameterizedString, T>)
        static void visit(std::unique_ptr<T>& object, TContext& context)
        {
            TPolicies::ValuePolicy::process(object->getTemplate(), context);
            object->updateTemplate();
        }
    };

    template<typename TPolicies>
    using ObjectVisitorExtensions = generic::TypeList<GlmObjectVisitor<TPolicies>,
            struct ParameterizedStringObjectVisitor<TPolicies>>;

    template<typename T, typename TPolicy, typename TState>
    class YamlSerializerGeneric: public implementation::ImplementationLevel<TPolicy, TState>
    {
    public:
        inline std::string serialize(const T& object) final
        {
            return dory::serialization::yaml::serialize<const T, ObjectVisitorExtensions<dory::serialization::yaml::YamlSerializationPolicies>>(object);
        }

        inline void deserialize(const std::string& source, T& object) final
        {
            dory::serialization::yaml::deserialize<T, ObjectVisitorExtensions<dory::serialization::yaml::YamlDeserializationPolicies>>(source, object);
        }
    };

    struct YamlSerializerPolicy: implementation::ImplementationPolicy<implementation::ImplementationList<YamlSerializerGeneric>>
    {};

    using YamlSerializer = implementation::Implementation<generic::TypeList<ISerializer>, ISerializer::Types, YamlSerializerPolicy>;

    template<typename T, typename TPolicy, typename TState>
    class JsonSerializerGeneric: public implementation::ImplementationLevel<TPolicy, TState>
    {
    public:
        inline std::string serialize(const T& object) final
        {
            return dory::serialization::json::serialize<const T, ObjectVisitorExtensions<dory::serialization::json::JsonSerializationPolicies>>(object);
        }

        inline void deserialize(const std::string& source, T& object) final
        {
            dory::serialization::json::deserialize<T, ObjectVisitorExtensions<dory::serialization::json::JsonDeserializationPolicies>>(source, object);
        }
    };

    struct JsonSerializerPolicy: implementation::ImplementationPolicy<implementation::ImplementationList<JsonSerializerGeneric>>
    {};

    using JsonSerializer = implementation::Implementation<generic::TypeList<ISerializer>, ISerializer::Types, JsonSerializerPolicy>;
}
