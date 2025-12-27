#pragma once

#include <dory/core/services/iSerializer.h>
#include <dory/core/implementation.h>
#include <dory/core/resources/factory.h>
#include <dory/serialization/yamlSerializer.h>
#include <dory/serialization/yamlDeserializer.h>
#include <dory/serialization/jsonSerializer.h>
#include <dory/serialization/jsonDeserializer.h>
#include <dory/containers/hashId.h>

namespace dory::core::services::serialization
{
    template<typename TPolicies>
    struct GlmObjectVisitor;

    template<typename TPolicies>
    struct ParameterizedStringObjectVisitor;

    template<typename TPolicies>
    struct ClassInstanceFactoryObjectVisitor;

    template<typename TPolicies>
    using ObjectVisitorExtensions = generic::TypeList<
            GlmObjectVisitor<TPolicies>,
            ParameterizedStringObjectVisitor<TPolicies>,
            ClassInstanceFactoryObjectVisitor<TPolicies>>;

    //GLM extensions to the base ObjectVisitor
    template<typename TPolicies>
    struct GlmObjectVisitor
    {
        using VisitorType = dory::serialization::ObjectVisitor<TPolicies, ObjectVisitorExtensions<TPolicies>>;

        template<typename T, auto size, typename TContext>
        static void visit(glm::vec<size, T>& vector, TContext& context)
        {
            dory::serialization::ArrayVisitor<T, size, TPolicies, VisitorType>::visit(vector, context);
        }

        template<typename T, auto size, typename TContext>
        static void visit(const glm::vec<size, T>& vector, TContext& context)
        {
            dory::serialization::ArrayVisitor<const T, size, TPolicies, VisitorType>::visit(vector, context);
        }

        constexpr static std::size_t quaternionSize = 4;

        template<typename T, typename TContext>
        static void visit(glm::qua<T>& vector, TContext& context)
        {
            dory::serialization::ArrayVisitor<T, quaternionSize, TPolicies, VisitorType>::visit(vector, context);
        }

        template<typename T, typename TContext>
        static void visit(const glm::qua<T>& vector, TContext& context)
        {
            dory::serialization::ArrayVisitor<const T, quaternionSize, TPolicies, VisitorType>::visit(vector, context);
        }
    };

    //Localization extensions to the base ObjectVisitor
    template<typename TPolicies>
    struct ParameterizedStringObjectVisitor
    {
        using VisitorType = dory::serialization::ObjectVisitor<TPolicies, ObjectVisitorExtensions<TPolicies>>;

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

        template<typename T, typename TContext>
        requires(std::is_base_of_v<generic::IParameterizedString, std::decay_t<T>>)
        static void visit(T* object, TContext& context)
        {
            TPolicies::ValuePolicy::process(object->getTemplate(), context);
            object->updateTemplate();
        }
    };

    //ClassInstanceFactory extensions to the base ObjectVisitor
    template<typename TPolicies>
    struct ClassInstanceFactoryObjectVisitor
    {
        using VisitorType = dory::serialization::ObjectVisitor<TPolicies, ObjectVisitorExtensions<TPolicies>>;

        template<typename TInstance, typename TContext>
        static void visit(resources::factory::Instance<TInstance>& factoryInstance, TContext& context)
        {
            dory::serialization::ClassVisitor<TPolicies, VisitorType>::visit(factoryInstance, context);

            auto type = containers::hash::hash(factoryInstance.type);
            if(auto factory = context.registry.template get<IObjectFactory<TInstance>>(type))
            {
                factoryInstance.instance = factory->createObject(&context);
            }
        }

        template<typename TInstance, typename TContext>
        static void visit(const resources::factory::Instance<TInstance>& factoryInstance, TContext& context)
        {
            dory::serialization::ClassVisitor<TPolicies, VisitorType>::visit(factoryInstance, context);

            if(auto instance = factoryInstance.instance.lock())
            {
                dory::serialization::ClassVisitor<TPolicies, VisitorType>::visit(*instance, context);
            }
        }
    };

    template<typename T, typename TPolicy, typename TState>
    class YamlSerializerGeneric: public implementation::ImplementationLevel<TPolicy, TState>
    {
    public:
        std::string serialize(const T& object, Registry& registry, resources::DataContext& dataContext) final
        {
            auto contextBase = dory::generic::serialization::Context<SerializationContextPoliciesType>{ registry, dataContext, resources::DataFormat::yaml };

            return dory::serialization::yaml::serialize<const T, SerializationContextPoliciesType,
                ObjectVisitorExtensions<dory::serialization::yaml::YamlSerializationPolicies>>(object, std::move(contextBase));
        }

        void deserialize(const std::string& source, T& object, Registry& registry, resources::DataContext& dataContext) final
        {
            auto contextBase = dory::generic::serialization::Context<SerializationContextPoliciesType>{ registry, dataContext, resources::DataFormat::yaml };

            dory::serialization::yaml::deserialize<T, SerializationContextPoliciesType,
                ObjectVisitorExtensions<dory::serialization::yaml::YamlDeserializationPolicies>>(source, object, std::move(contextBase));
        }

        void deserialize(T& object, generic::serialization::Context<SerializationContextPoliciesType>& context) final
        {
            assert(context.dataFormat == resources::DataFormat::yaml);

            auto& yamlContext = static_cast<dory::serialization::yaml::YamlContext<SerializationContextPoliciesType>&>(context);
            dory::serialization::yaml::deserialize<T, SerializationContextPoliciesType,
                ObjectVisitorExtensions<dory::serialization::yaml::YamlDeserializationPolicies>>(object, yamlContext);
        }
    };

    struct YamlSerializerPolicy: implementation::ImplementationPolicy<implementation::ImplementationList<YamlSerializerGeneric>>
    {};

    using YamlSerializer = implementation::Implementation<generic::TypeList<ISerializer>, ISerializer::Types, YamlSerializerPolicy>;

    template<typename T, typename TPolicy, typename TState>
    class JsonSerializerGeneric: public implementation::ImplementationLevel<TPolicy, TState>
    {
    public:
        std::string serialize(const T& object, Registry& registry, resources::DataContext& dataContext) final
        {
            auto contextBase = dory::generic::serialization::Context<SerializationContextPoliciesType>{ registry, dataContext, resources::DataFormat::json };

            return dory::serialization::json::serialize<const T, SerializationContextPoliciesType,
                ObjectVisitorExtensions<dory::serialization::json::JsonSerializationPolicies>>(object, std::move(contextBase));
        }

        void deserialize(const std::string& source, T& object, Registry& registry, resources::DataContext& dataContext) final
        {
            auto contextBase = dory::generic::serialization::Context<SerializationContextPoliciesType>{ registry, dataContext, resources::DataFormat::json };

            dory::serialization::json::deserialize<T, SerializationContextPoliciesType,
                ObjectVisitorExtensions<dory::serialization::json::JsonDeserializationPolicies>>(source, object, std::move(contextBase));
        }

        void deserialize(T& object, generic::serialization::Context<SerializationContextPoliciesType>& context) final
        {
            assert(context.dataFormat == resources::DataFormat::json);

            auto& jsonContext = static_cast<dory::serialization::json::JsonContext<SerializationContextPoliciesType>&>(context);
            dory::serialization::json::deserialize<T, SerializationContextPoliciesType,
                ObjectVisitorExtensions<dory::serialization::json::JsonDeserializationPolicies>>(object, jsonContext);
        }
    };

    struct JsonSerializerPolicy: implementation::ImplementationPolicy<implementation::ImplementationList<JsonSerializerGeneric>>
    {};

    using JsonSerializer = implementation::Implementation<generic::TypeList<ISerializer>, ISerializer::Types, JsonSerializerPolicy>;
}
