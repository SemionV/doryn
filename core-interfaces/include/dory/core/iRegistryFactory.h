#pragma once

#include "registry.h"

namespace dory::core
{
    class IRegistryFactory
    {
    public:
        virtual ~IRegistryFactory() = default;

        virtual std::unique_ptr<Registry> createRegistry() = 0;
    };
}