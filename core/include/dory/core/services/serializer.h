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
    struct ObjectVisitor
    {
        using PoliciesType = TPolicies;
        using VisitorType = dory::serialization::ObjectVisitor<PoliciesType, ObjectVisitor>;

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

    template<typename T, typename TPolicy, typename TState>
    class YamlSerializerGeneric: public implementation::ImplementationLevel<TPolicy, TState>
    {
    public:
        inline std::string serialize(const T& object) final
        {
            return dory::serialization::yaml::serialize<const T, ObjectVisitor<dory::serialization::yaml::YamlDeserializationPolicies>>(object);
        }

        inline void deserialize(const std::string& source, T& object) final
        {
            dory::serialization::yaml::deserialize<T, ObjectVisitor<dory::serialization::yaml::YamlDeserializationPolicies>>(source, object);
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
            return dory::serialization::json::serialize<const T, ObjectVisitor<dory::serialization::json::JsonSerializationPolicies>>(object);
        }

        inline void deserialize(const std::string& source, T& object) final
        {
            dory::serialization::json::deserialize<T, ObjectVisitor<dory::serialization::json::JsonDeserializationPolicies>>(source, object);
        }
    };

    struct JsonSerializerPolicy: implementation::ImplementationPolicy<implementation::ImplementationList<JsonSerializerGeneric>>
    {};

    using JsonSerializer = implementation::Implementation<generic::TypeList<ISerializer>, ISerializer::Types, JsonSerializerPolicy>;
}
