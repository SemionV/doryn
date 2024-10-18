#include "dory/game/engine/setup.h"
#include <iostream>

#ifdef DORY_MAIN_FUNCTION_UNIX
int main()
#endif
#ifdef DORY_MAIN_FUNCTION_WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
#endif
{
    dory::game::engine::Setup setup;
    dory::core::extensionPlatform::LibraryHandle staticLibraryHandle {};
    auto registry = dory::core::Registry{};
    auto dataContext = dory::core::resources::DataContext{};

    setup.setupRegistry(staticLibraryHandle, registry);

    {
        auto fileService = registry.get<dory::core::services::IFileService>();
        if(fileService)
        {
            std::cout << fileService->getMessage() << "\n";
        }
    }

    {
        auto eventHub = registry.get<dory::core::events::script::IEventHub>();
        if(eventHub)
        {
            eventHub->attach([](dory::core::resources::DataContext& dataContext, const dory::core::events::script::Run& event)
                {
                    std::cout << "script event is fired\n";
                });
        }
    }

    {
        auto libraryService = registry.get<dory::core::services::ILibraryService>();
        if(libraryService)
        {
            auto testExtensionLibrary = libraryService->load("test extension library", "modules/test-extension");
            if(testExtensionLibrary)
            {
                auto extension = testExtensionLibrary->loadModule("test-extension", registry);
                if(extension)
                {
                    extension->attach(dory::core::extensionPlatform::LibraryHandle{testExtensionLibrary }, dataContext);

                    auto fileService = registry.get<dory::core::services::IFileService>();
                    if(fileService)
                    {
                        auto message = fileService->getMessage();
                        std::cout << message << "\n";
                    }
                }
            }
        }
    }

    {
        auto libraryService = registry.get<dory::core::services::ILibraryService>();
        if(libraryService)
        {
            libraryService->unload("test extension library");
        }
    }

    std::cout << "End main" << std::endl;

    return 0;
}