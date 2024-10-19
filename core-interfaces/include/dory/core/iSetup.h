#pragma once

#include "registry.h"

namespace dory::core
{
    class ISetup
    {
    public:
        virtual ~ISetup() = default;

        virtual void setupRegistry(const extensionPlatform::LibraryHandle& libraryHandle, Registry& registry) = 0;
    };
}