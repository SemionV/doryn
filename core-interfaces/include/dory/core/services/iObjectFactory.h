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
        virtual std::unique_ptr<T> createInstance(generic::serialization::Context<Registry, resources::DataContext> context) = 0;
    };
}