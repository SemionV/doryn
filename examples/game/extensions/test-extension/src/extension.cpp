#include <extension.h>
#include <iostream>

namespace dory::game::test_extension
{
    void dory::game::test_extension::Extension::attach(dory::core::extensionPlatform::LibraryHandle library, const dory::core::resources::ExtensionContext& extensionContext)
    {
        _registry = extensionContext.registry;

        std::cout << "dory::game::test_extension::Extension: Attach extension\n";
    }

    Extension::~Extension()
    {
        std::cout << "dory::game::test_extension::Extension: Detach extension\n";
    }
}