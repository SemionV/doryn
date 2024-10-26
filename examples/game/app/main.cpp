#include "dory/game/engine/setup.h"
#include <iostream>
#include <dory/core/services/logService.h>
#include <dory/core/resources/localizationImpl.h>

#ifdef DORY_MAIN_FUNCTION_UNIX
int main()
#endif
#ifdef DORY_MAIN_FUNCTION_WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
#endif
{
    dory::game::engine::Setup setup;
    dory::generic::extension::LibraryHandle staticLibraryHandle {};
    auto registry = dory::core::Registry{};
    auto dataContext = dory::core::resources::DataContext{};
    auto configuration = dory::core::resources::configuration::Configuration{};
    auto localization = dory::core::resources::LocalizationImpl{};

    setup.setupRegistry(staticLibraryHandle, registry);

    //initial config
    configuration.section.loadFrom.emplace_back("settings.yaml");
    auto& bootLoggerConfig = configuration.loggingConfiguration.configurationLogger;
    bootLoggerConfig.name = "boot";
    bootLoggerConfig.rotationLogger = dory::core::resources::configuration::RotationLogSink{"logs/boot.log"};
    bootLoggerConfig.stdoutLogger = dory::core::resources::configuration::StdoutLogSink{};

    registry.get<dory::core::services::IMultiSinkLogService>(dory::core::resources::Logger::Config, [&bootLoggerConfig, &registry](dory::core::services::IMultiSinkLogService* logger)
    {
        logger->initialize(bootLoggerConfig, registry);
    });

    registry.get<dory::core::services::IConfigurationService>([&configuration](dory::core::services::IConfigurationService* configurationService)
    {
        configurationService->load(configuration);
    });

    registry.get<dory::core::services::IMultiSinkLogService, dory::core::resources::Logger::App>([&configuration, &registry](dory::core::services::IMultiSinkLogService* logger)
    {
        logger->initialize(configuration.loggingConfiguration.mainLogger, registry);
    });

    registry.get<dory::core::services::ILogService, dory::core::resources::Logger::App>([&configuration](dory::core::services::ILogService* logger)
    {
        logger->information(fmt::format("Dory Game, {0}.{1}, {2}",
                                                            configuration.buildInfo.version,
                                                            configuration.buildInfo.commitSHA,
                                                            configuration.buildInfo.timestamp));
    });

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

    {
        auto libraryService = registry.get<dory::core::services::ILibraryService>();
        if(libraryService)
        {
            libraryService->unload("test extension library");
        }
    }

    {
        registry.get<dory::core::devices::IStandardIODevice>([&dataContext](dory::core::devices::IStandardIODevice* device)
        {
            device->disconnect(dataContext);
        });
    }

    /*{
        registry.get<dory::core::services::IMultiSinkLogService, dory::core::resources::Logger::App>([&configuration, &registry](dory::core::services::IMultiSinkLogService* logger)
        {
            configuration.loggingConfiguration.mainLogger.name = "AppLogger";
            configuration.loggingConfiguration.mainLogger.stdoutLogger = dory::core::resources::configuration::StdoutLogSink{};
            logger->initialize(configuration.loggingConfiguration.mainLogger, registry);
            logger->information(std::string{"Hello from AppLogger!"});
        });

        registry.get<dory::core::services::IMultiSinkLogService>(dory::core::resources::Logger::Config, [&configuration, &registry](dory::core::services::IMultiSinkLogService* logger)
        {
            configuration.loggingConfiguration.mainLogger.name = "ConfigLogger";
            configuration.loggingConfiguration.mainLogger.stdoutLogger = dory::core::resources::configuration::StdoutLogSink{};
            logger->initialize(configuration.loggingConfiguration.mainLogger, registry);
            logger->information(std::string{"Hello from ConfigLogger!"});
        });
    }*/

    std::cout << "End main" << std::endl;

    return 0;
}