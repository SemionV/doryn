#pragma once

#include <dory/core/services/iObjectFactory.h>

namespace dory::core::services
{
    template<typename TInterface, typename TImplementation>
    requires(std::is_base_of_v<TInterface, TImplementation>)
    class ObjectFactory: public IObjectFactory<TInterface>
    {
    public:
        std::unique_ptr<TInterface> createInstance(generic::serialization::Context<typename IObjectFactory<TInterface>::SerializationContextPoliciesType>& context) final
        {
            auto instance = std::make_unique<TImplementation>(context.registry);

            if(auto serializer = context.registry.template get<serialization::ISerializer>(context.dataFormat))
            {
                serializer->deserialize(instance, context);
            }

            return instance;
        }
    };
}
