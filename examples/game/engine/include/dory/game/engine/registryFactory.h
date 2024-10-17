#pragma once

#include <dory/core/iRegistryFactory.h>
#include <dory/core/macros.h>

namespace dory::game::engine
{
    class DORY_DLLEXPORT RegistryFactory: public core::IRegistryFactory
    {
    public:
        core::Registry createRegistry(const core::extensionPlatform::LibraryHandle& libraryHandle) override;
    };
}