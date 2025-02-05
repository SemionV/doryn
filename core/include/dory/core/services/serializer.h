#pragma once

#include <dory/core/services/iSerializer.h>
#include <dory/core/implementation.h>
#include <dory/core/resources/serialization.h>
#include <dory/serialization/yamlSerializer.h>
#include <dory/serialization/yamlDeserializer.h>
#include <dory/serialization/jsonSerializer.h>
#include <dory/serialization/jsonDeserializer.h>

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
    };

    //TODO:
    // Create a base class for all serialization Contexts and put Registry&, DataContext& and DataFormat fields to it
    // Add serialization/deserialization methods to all sorts of serializers, which are getting an existing serialization Context as a parameter
    // Implement Factory as a templated class, which should be registered in the service container relative to a string key and a TInstance specialization
    // The Factory implementation takes a reference to the base serialization Context class and searches for a serializer according to DataFormat value
    // The Factory either allocates a new object of type TInstance and sends it down to a corresponding deserializator together with deserialization context
    // or takes the existing instance of TInstance and send is down to a corresponding serialization together with the serialization Context

    //ControllerFactory extensions to the base ObjectVisitor
    template<typename TPolicies>
    struct ClassInstanceFactoryObjectVisitor
    {
        using VisitorType = dory::serialization::ObjectVisitor<TPolicies, ObjectVisitorExtensions<TPolicies>>;

        template<typename TInstance, typename TContext>
        static void visit(resources::serialization::FactoryInstance<TInstance>& factoryInstance, TContext& context)
        {
            dory::serialization::ClassVisitor<TPolicies, VisitorType>::visit(factoryInstance, context);

            if(auto factory = context.registry.template get<IObjectFactory<TInstance>>(factoryInstance.type))
            {
                factoryInstance.instance = factory->createInstance(context);
            }

            if(factoryInstance.instance)
            {
                dory::serialization::ClassVisitor<TPolicies, VisitorType>::visit(factoryInstance.instance, context);
            }
        }

        template<typename TInstance, typename TContext>
        static void visit(const resources::serialization::FactoryInstance<TInstance>& factoryInstance, TContext& context)
        {
            dory::serialization::ClassVisitor<TPolicies, VisitorType>::visit(factoryInstance, context);

            if(factoryInstance.instance)
            {
                dory::serialization::ClassVisitor<TPolicies, VisitorType>::visit(factoryInstance.instance, context);
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
            auto contextBase = dory::generic::serialization::Context<SerializationContextPoliciesType>{ registry, dataContext, resources::DataFormat::yaml };

            return dory::serialization::json::serialize<const T, SerializationContextPoliciesType,
                ObjectVisitorExtensions<dory::serialization::json::JsonSerializationPolicies>>(object, std::move(contextBase));
        }

        void deserialize(const std::string& source, T& object, Registry& registry, resources::DataContext& dataContext) final
        {
            auto contextBase = dory::generic::serialization::Context<SerializationContextPoliciesType>{ registry, dataContext, resources::DataFormat::yaml };

            dory::serialization::json::deserialize<T, SerializationContextPoliciesType,
                ObjectVisitorExtensions<dory::serialization::json::JsonDeserializationPolicies>>(source, object, std::move(contextBase));
        }
    };

    struct JsonSerializerPolicy: implementation::ImplementationPolicy<implementation::ImplementationList<JsonSerializerGeneric>>
    {};

    using JsonSerializer = implementation::Implementation<generic::TypeList<ISerializer>, ISerializer::Types, JsonSerializerPolicy>;
}
