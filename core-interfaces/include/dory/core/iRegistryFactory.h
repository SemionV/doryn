#pragma once

#include "registry.h"

namespace dory::core
{
    class IRegistryFactory
    {
    public:
        virtual ~IRegistryFactory() = default;

        virtual Registry createRegistry(const extensionPlatform::LibraryHandle& libraryHandle) = 0;
    };
}