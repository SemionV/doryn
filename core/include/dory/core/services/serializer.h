#pragma once

#include <dory/core/services/iSerializer.h>
#include <dory/core/implementation.h>
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
    using ObjectVisitorExtensions = generic::TypeList<GlmObjectVisitor<TPolicies>,
            struct ParameterizedStringObjectVisitor<TPolicies>>;

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

    //TODO: move to dory::core::resources namespace
    template<typename TInstance, typename TFactory>
    struct FactoryInstance
    {
        std::unique_ptr<TInstance> instance;
        std::string factoryKey {};
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
    struct ControllerFactoryObjectVisitor
    {
        using VisitorType = dory::serialization::ObjectVisitor<TPolicies, ObjectVisitorExtensions<TPolicies>>;

        template<typename TInstance, typename TFactory, typename TContext>
        static void visit(FactoryInstance<TInstance, TFactory>& factoryInstance, TContext& context)
        {
            dory::serialization::ClassVisitor<TPolicies, VisitorType>::visit(factoryInstance, context);

            if(auto factory = context.registry.template get<TFactory>(factoryInstance.factoryKey))
            {
                factoryInstance.instance = factory->template loadInstance<TInstance>(context);
            }
        }

        template<typename TInstance, typename TFactory, typename TContext>
        static void visit(const FactoryInstance<TInstance, TFactory>& factoryInstance, TContext& context)
        {
            dory::serialization::ClassVisitor<TPolicies, VisitorType>::visit(factoryInstance, context);

            if(auto factory = context.registry.template get<TFactory>(factoryInstance.factoryKey))
            {
                factory->template saveInstance<TInstance>(factoryInstance.instance, context);
            }
        }
    };

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
