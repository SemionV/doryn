#include "dory/game/setup.h"
#include <iostream>
#include <dory/core/services/logService.h>
#include <dory/core/resources/localizationImpl.h>
#include <dory/game/bootstrap.h>

#ifdef DORY_MAIN_FUNCTION_UNIX
int main()
#endif
#ifdef DORY_MAIN_FUNCTION_WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
#endif
{
    dory::game::Setup setup;
    dory::generic::extension::LibraryHandle staticLibraryHandle {};
    auto registry = dory::core::Registry{};
    auto configuration = dory::core::resources::configuration::Configuration{};
    auto localization = dory::core::resources::LocalizationImpl{};
    auto dataContext = dory::core::resources::DataContext{ configuration, localization };

    setup.setupRegistry(staticLibraryHandle, registry);

    //initial config
    configuration.section.loadFrom.emplace_back("settings.yaml");
    auto& bootLoggerConfig = configuration.loggingConfiguration.configurationLogger;
    bootLoggerConfig.name = "boot";
    bootLoggerConfig.rotationLogger = dory::core::resources::configuration::RotationLogSink{"logs/boot.log"};
    bootLoggerConfig.stdoutLogger = dory::core::resources::configuration::StdoutLogSink{};

    auto bootstrap = dory::game::Bootstrap{};
    bootstrap.initialize(staticLibraryHandle, dataContext, registry);

    bootstrap.run(dataContext, registry);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    {
        auto eventHub = registry.get<dory::core::events::script::Bundle::IListener>();
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
                    extension->attach(dory::generic::extension::LibraryHandle{testExtensionLibrary }, dataContext);

                    auto fileService = registry.get<dory::core::services::IFileService>();
                    if(fileService)
                    {
                        //auto message = fileService->getMessage();
                        //std::cout << message << "\n";
                    }
                }
            }
        }
    }

    std::cout << "End main" << std::endl;

    return 0;
}