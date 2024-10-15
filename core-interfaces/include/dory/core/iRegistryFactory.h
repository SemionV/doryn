#pragma once

#include "registry.h"

namespace dory::core
{
    class IRegistryFactory
    {
    public:
        virtual ~IRegistryFactory() = default;

        virtual std::shared_ptr<Registry> createRegistry() = 0;
    };
}