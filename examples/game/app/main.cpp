#include "dory/game/engine/registryFactory.h"
#include <iostream>

#ifdef DORY_MAIN_FUNCTION_UNIX
int main()
#endif
#ifdef DORY_MAIN_FUNCTION_WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
#endif
{
    dory::game::engine::RegistryFactory factory;

    auto registry = factory.createRegistry();
    std::cout << registry.services.getFileService()->getMessage() << "\n";

    auto dataContext = dory::core::resources::DataContext{};

    registry.events.scriptHub->attach([](dory::core::resources::DataContext& dataContext, const dory::core::events::script::Run& event)
        {
            std::cout << "script event is fired\n";
        });

    auto testExtensionLibrary = registry.services.libraryService->load("test extension library", "modules/test-extension");
    if(testExtensionLibrary)
    {
        auto extension = testExtensionLibrary->loadModule("test-extension");
        if(extension)
        {
            extension->attach(dory::core::extensionPlatform::LibraryHandle{testExtensionLibrary }, dataContext, registry);

            auto fileServiceRef = registry.services.getFileService();
            if(fileServiceRef)
            {
                auto message = fileServiceRef->getMessage();
                std::cout << message << "\n";
            }
        }
    }

    return 0;
}