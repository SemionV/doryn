#include <extension.h>
#include <iostream>
#include <dory/core/registry.h>

namespace dory::game::test_extension
{
    void dory::game::test_extension::Extension::attach(dory::core::extensionPlatform::LibraryHandle library, const dory::core::resources::ExtensionContext& extensionContext)
    {
        _registry = extensionContext.registry;

        std::cout << "dory::game::test_extension::Extension: Attach extension\n";

        auto dataContext = core::resources::DataContext{};
        extensionContext.registry->events.scriptDispatcher->fire(dataContext, core::events::script::Run{"test-script"});
    }

    Extension::~Extension()
    {
        std::cout << "dory::game::test_extension::Extension: Detach extension\n";
    }
}