#pragma once

#include <dory/core/extensionPlatform/iModule.h>

namespace dory::game::test_extension
{
    class Extension: public dory::core::extensionPlatform::IExtensionModule
    {
    private:
        std::weak_ptr<core::Registry> _registry;

    public:
        ~Extension() override;

        void attach(dory::core::extensionPlatform::LibraryHandle library, const dory::core::resources::ExtensionContext& extensionContext) final;
    };
}