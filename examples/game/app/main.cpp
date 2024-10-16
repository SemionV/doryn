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
    std::cout << registry->services.fileService->getMessage() << "\n";

    registry->events.scriptHub.attach(std::function<void(dory::core::resources::DataContext&, const dory::core::events::script::Run&)>{[](dory::core::resources::DataContext& dataContext, const dory::core::events::script::Run& event)
                                      {
                                          std::cout << "script even is fired\n";
                                      }});

    auto testExtensionLibrary = registry->services.libraryService->load("test extension library", "modules/test-extension");
    if(testExtensionLibrary)
    {
        auto extensionContext = dory::core::resources::ExtensionContext{ registry };

        auto extension = testExtensionLibrary->loadExtensionModule("test-extension");
        if(extension)
        {
            extension->attach(dory::core::extensionPlatform::LibraryHandle{testExtensionLibrary }, extensionContext);
        }
    }

    return 0;
}