#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/generic/serialization.h>

namespace dory::core
{
    struct Registry;
}

namespace dory::core::services
{
    template<typename T>
    class IObjectFactory: public generic::Interface
    {
    public:
        using SerializationContextPoliciesType = generic::serialization::ContextPolicies<Registry, resources::DataContext, resources::DataFormat>;

        virtual std::unique_ptr<T> createInstance(generic::serialization::Context<SerializationContextPoliciesType>& context) = 0;
    };
}