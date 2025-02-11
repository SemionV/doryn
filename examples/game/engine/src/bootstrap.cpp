#include <dory/core/registry.h>
#include "dory/game/bootstrap.h"
#include <regex>
#include <thread>
#include <spdlog/fmt/fmt.h>

namespace dory::game
{
    using namespace core;
    using namespace resources;
    using namespace entities;
    using namespace repositories;
    using namespace services;
    using namespace devices;
    using namespace dory::generic::extension;
    using namespace generic::extension;
    using namespace generic::registry;

    std::string threadIdToString(const std::thread::id& id)
    {
        std::stringstream ss;
        ss << id;
        return ss.str();
    }

    Bootstrap::Bootstrap(Registry &registry):
        _registry(registry)
    {}

    bool Bootstrap::initialize(const LibraryHandle& libraryHandle, DataContext& context)
    {
        loadConfiguration(libraryHandle, context);
        loadExtensions(libraryHandle, context);

        if(auto logger = _registry.get<ILogService, Logger::Config>())
        {
            logger->information(fmt::format("main threadId: {}", threadIdToString(std::this_thread::get_id())));
        }

        return true;
    }

    bool Bootstrap::run(DataContext& context)
    {
        _registry.get<ILoopService>([&context](ILoopService* loopService) {
            loopService->startLoop(context);
        });

        return true;
    }

    void Bootstrap::cleanup(DataContext &context)
    {
        _registry.get<Service<ILogService, Logger::App>, Service<ITerminalDevice>, Service<IStandardIODevice>, Service<IFileWatcherDevice>,
                                Service<IImageStreamDevice>, Service<ILibraryService>>
                ([&context](ILogService* logger, ITerminalDevice* terminalDevice, IStandardIODevice* ioDevice, IFileWatcherDevice* fileWatcherDevice,
                                IImageStreamDevice* imageStreamDevice, ILibraryService* libraryService)
                {
                    terminalDevice->exitCommandMode();
                    logger->information(std::string_view {"Cleanup..."});
                    terminalDevice->disconnect(context);
                    ioDevice->disconnect(context);
                    fileWatcherDevice->disconnect(context);
                    imageStreamDevice->disconnect(context);

                    libraryService->unloadAll();
                });

        _registry.getAll<IWindowSystemDevice, WindowSystem>([&context](const auto& devices) {
            for(const auto& [key, value] : devices)
            {
                auto deviceRef = value.lock();
                if(deviceRef)
                {
                    deviceRef->disconnect(context);
                }
            }
        });
    }

    void Bootstrap::loadConfiguration(const LibraryHandle& libraryHandle, DataContext& context) const
    {
        _registry.get<IMultiSinkLogService>(Logger::Config, [this, &context](IMultiSinkLogService* logger){
            logger->initialize(context.configuration.loggingConfiguration.configurationLogger, _registry);
        });

        _registry.get<IConfigurationService>([&context](IConfigurationService* configurationService){
            configurationService->load(context.configuration, context);
        });

        _registry.get<IMultiSinkLogService, Logger::App>([this, &context](IMultiSinkLogService* logger){
            logger->initialize(context.configuration.loggingConfiguration.mainLogger, _registry);
        });

        _registry.get<ILocalizationService>([&context](ILocalizationService* localizationService){
            localizationService->load(context.configuration, context.localization, context);
        });
    }

    void Bootstrap::loadExtensions(const LibraryHandle& libraryHandle, DataContext& context) const
    {
        _registry.get<ILibraryService>([&context](ILibraryService* libraryService) {
            for(const auto& extension : context.configuration.extensions)
            {
                libraryService->load(context, extension);
            }
        });
    }
}