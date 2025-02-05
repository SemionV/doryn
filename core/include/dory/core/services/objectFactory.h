#pragma once

#include <dory/core/services/iObjectFactory.h>

namespace dory::core::services
{
    template<typename TInterface, typename TImplementation>
    requires(std::is_base_of_v<TInterface, TImplementation>)
    class ObjectFactory: public IObjectFactory<TInterface>
    {
    private:
        const generic::extension::LibraryHandle& _libraryHandle;

    public:
        explicit ObjectFactory(const generic::extension::LibraryHandle& libraryHandle):
            _libraryHandle(libraryHandle)
        {}

        generic::extension::ResourceHandle<std::shared_ptr<TInterface>> createInstance(generic::serialization::Context<typename IObjectFactory<TInterface>::SerializationContextPoliciesType>& context) final
        {
            auto instance = std::make_shared<TImplementation>(context.registry);

            if(auto serializer = context.registry.template get<serialization::ISerializer>(context.dataFormat))
            {
                serializer->deserialize(*instance.get(), context);
            }

            return generic::extension::ResourceHandle<std::shared_ptr<TInterface>>{ _libraryHandle, instance };
        }
    };
}