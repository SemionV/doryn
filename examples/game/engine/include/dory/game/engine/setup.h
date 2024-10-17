#pragma once

#include <dory/core/iSetup.h>
#include <dory/core/macros.h>

namespace dory::game::engine
{
    class DORY_DLLEXPORT Setup: public core::ISetup
    {
    public:
        void setupRegistry(const core::extensionPlatform::LibraryHandle& libraryHandle, core::Registry& registry) override;
    };
}