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
        if(auto libraryService = _registry.get<ILibraryService>())
        {
            libraryService->unloadAll();
        }
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
        if(auto libraryService = _registry.get<ILibraryService>())
        {
            for(const auto& extension : context.configuration.extensions)
            {
                libraryService->load(context, extension);
            }
        }
    }
}