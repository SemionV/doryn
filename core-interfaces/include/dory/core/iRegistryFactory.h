#pragma once

#include "registry.h"

namespace dory::core
{
    class IRegistryFactory
    {
    public:
        virtual ~IRegistryFactory() = default;

        virtual Registry createRegistry() = 0;
    };
}