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
    auto config = dory::core::resources::configuration::Configuration{};

    setup.setupRegistry(staticLibraryHandle, registry);

    {
        auto ioDevice = registry.get<dory::core::devices::IStandardIODevice>();
        if(ioDevice)
        {
            ioDevice->connect(dataContext);
            ioDevice->out("Hello from IODevice!\n");
        }
    }

    {
        auto terminalDevice = registry.get<dory::core::devices::ITerminalDevice>();
        if(terminalDevice)
        {
            terminalDevice->connect(dataContext);
            terminalDevice->enterCommandMode();
            terminalDevice->writeLine("Hello Terminal!");
            terminalDevice->exitCommandMode();
        }
    }

    {
        auto fileService = registry.get<dory::core::services::IFileService>();
        if(fileService)
        {
            std::cout << fileService->getMessage() << "\n";
        }
    }

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

    {
        auto ioDevice = registry.get<dory::core::devices::IStandardIODevice>();
        if(ioDevice)
        {
            ioDevice->disconnect(dataContext);
        }
    }

    {
        auto logger = registry.get<dory::core::services::IMultiSinkLogService, dory::core::Logger::App>();
        if(logger)
        {
            config.loggingConfiguration.mainLogger.name = "AppLogger";
            config.loggingConfiguration.mainLogger.stdoutLogger = dory::core::resources::configuration::StdoutLogSink{};
            logger->initialize(config.loggingConfiguration.mainLogger, registry);
            logger->information(std::string{"Hello from AppLogger!"});
        }

        auto logger2 = registry.get<dory::core::services::IMultiSinkLogService, dory::core::Logger::Config>();
        if(logger2)
        {
            config.loggingConfiguration.mainLogger.name = "ConfigLogger";
            config.loggingConfiguration.mainLogger.stdoutLogger = dory::core::resources::configuration::StdoutLogSink{};
            logger2->initialize(config.loggingConfiguration.mainLogger, registry);
            logger2->information(std::string{"Hello from ConfigLogger!"});
        }
    }

    std::cout << "End main" << std::endl;

    return 0;
}