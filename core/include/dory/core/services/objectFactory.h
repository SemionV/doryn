#pragma once

#include <dory/core/services/iObjectFactory.h>
#include <dory/serialization/yamlSerializer.h>
#include <dory/serialization/jsonSerializer.h>
#include <dory/core/services/serializer.h>
#include <dory/core/dependencyResolver.h>

namespace dory::core::services
{
    template<typename TInterface, typename TImplementation>
    requires(std::is_base_of_v<TInterface, TImplementation>)
    struct ObjectDeserializator
    {
        using SerializationContextPoliciesType = typename IObjectFactory<TInterface>::SerializationContextPoliciesType;
        using YamlDeserializationPoliciesType = serialization::ObjectVisitorExtensions<dory::serialization::yaml::YamlDeserializationPolicies>;
        using JsonDeserializationPoliciesType = serialization::ObjectVisitorExtensions<dory::serialization::json::JsonDeserializationPolicies>;

        static void deserialize(std::shared_ptr<TImplementation>& instance, generic::serialization::Context<typename IObjectFactory<TInterface>::SerializationContextPoliciesType>& context)
        {
            if(context.dataFormat == resources::DataFormat::yaml)
            {
                auto& yamlContext = static_cast<dory::serialization::yaml::YamlContext<SerializationContextPoliciesType>&>(context);
                dory::serialization::yaml::deserialize<TImplementation, SerializationContextPoliciesType, YamlDeserializationPoliciesType>(*instance.get(), yamlContext);
            }
            else if(context.dataFormat == resources::DataFormat::json)
            {
                auto& jsonContext = static_cast<dory::serialization::json::JsonContext<SerializationContextPoliciesType>&>(context);
                dory::serialization::json::deserialize<TImplementation, SerializationContextPoliciesType, JsonDeserializationPoliciesType>(*instance.get(), jsonContext);
            }
        }
    };

    template<typename TInterface, typename TImplementation>
    requires(std::is_base_of_v<TInterface, TImplementation>)
    class ObjectFactory: public IObjectFactory<TInterface>, public DependencyResolver
    {
        static_assert(dory::reflection::IsReflectableV<TImplementation>);

    private:
        const generic::extension::LibraryHandle& _libraryHandle;

    public:
        explicit ObjectFactory(const generic::extension::LibraryHandle& libraryHandle, Registry& registry):
            DependencyResolver(registry),
            _libraryHandle(libraryHandle)
        {}

        generic::extension::ResourceHandle<std::shared_ptr<TInterface>> createObject(generic::serialization::Context<SerializationContextPoliciesType>* context) final
        {
            auto instance = std::make_shared<TImplementation>(_registry);

            if(context)
            {
                ObjectDeserializator<TInterface, TImplementation>::deserialize(instance, *context);
            }

            return generic::extension::ResourceHandle<std::shared_ptr<TInterface>>{ _libraryHandle, instance };
        }
    };

    template<typename TInterface, typename TInstanceInterface>
    class SingletonObjectFactory : public IObjectFactory<TInterface>, public DependencyResolver
    {
    public:
        explicit SingletonObjectFactory(Registry& registry):
            DependencyResolver(registry)
        {}

        generic::extension::ResourceHandle<std::shared_ptr<TInterface>> createObject(generic::serialization::Context<SerializationContextPoliciesType>* context) final
        {
            if(auto instanceHandle = _registry.getHandle<TInstanceInterface>())
            {
                return (*instanceHandle).clone();
            }
            return generic::extension::ResourceHandle<std::shared_ptr<TInterface>>{ generic::extension::LibraryHandle{}, nullptr };
        }
    };

    template<typename TInterface, typename TInstanceInterface, auto Identifier>
    class SingletonIdentifierObjectFactory : public IObjectFactory<TInterface>, public DependencyResolver
    {
    public:
        explicit SingletonIdentifierObjectFactory(Registry& registry):
            DependencyResolver(registry)
        {}

        generic::extension::ResourceHandle<std::shared_ptr<TInterface>> createObject(generic::serialization::Context<SerializationContextPoliciesType>* context) final
        {
            auto instanceHandle = _registry.getHandle<TInstanceInterface, Identifier>();
            return instanceHandle.template clone<std::shared_ptr<TInterface>>();
        }
    };
}