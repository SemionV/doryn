#pragma once

#include <dory/core/services/iObjectFactory.h>
#include <dory/serialization/yamlSerializer.h>
#include <dory/serialization/jsonSerializer.h>

namespace dory::core::services
{
    template<typename TInterface, typename TImplementation>
    requires(std::is_base_of_v<TInterface, TImplementation>)
    class ObjectFactory: public IObjectFactory<TInterface>
    {
        //Please add TImplementation to reflection database
        static_assert(dory::reflection::IsReflectableV<TImplementation>);

    private:
        const generic::extension::LibraryHandle& _libraryHandle;

    public:
        using SerializationContextPoliciesType = typename IObjectFactory<TInterface>::SerializationContextPoliciesType;
        using YamlDeserializationPoliciesType = serialization::ObjectVisitorExtensions<dory::serialization::yaml::YamlDeserializationPolicies>;
        using JsonDeserializationPoliciesType = serialization::ObjectVisitorExtensions<dory::serialization::json::JsonDeserializationPolicies>;

        explicit ObjectFactory(const generic::extension::LibraryHandle& libraryHandle):
            _libraryHandle(libraryHandle)
        {}

        generic::extension::ResourceHandle<std::shared_ptr<TInterface>> createInstance(generic::serialization::Context<typename IObjectFactory<TInterface>::SerializationContextPoliciesType>& context) final
        {
            auto instance = std::make_shared<TImplementation>(context.registry);

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

            return generic::extension::ResourceHandle<std::shared_ptr<TInterface>>{ _libraryHandle, instance };
        }
    };
}