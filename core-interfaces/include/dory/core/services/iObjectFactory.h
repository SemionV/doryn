#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/generic/serialization.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services
{
    template<typename T>
    class IObjectFactory: public generic::Interface
    {
    public:
        using SerializationContextPoliciesType = generic::serialization::ContextPolicies<Registry, resources::DataContext, resources::DataFormat>;

        virtual generic::extension::ResourceHandle<std::shared_ptr<T>> createInstance(generic::serialization::Context<SerializationContextPoliciesType>& context) = 0;
        virtual generic::extension::ResourceHandle<std::shared_ptr<T>> createNewInstance(Registry& registry) = 0;
    };
}