#include <dory/core/registry.h>
#include "dory/game/bootstrap.h"
#include <dory/core/controllers/windowSystemController.h>
#include <dory/core/controllers/accelerationMovementController.h>
#include <dory/core/controllers/movementController.h>
#include <dory/core/controllers/transformController.h>
#include <regex>
#include <iostream>
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
    using namespace controllers;
    using namespace dory::generic::extension;
    using namespace generic::extension;
    using namespace generic::registry;

    Bootstrap::Bootstrap(Registry &registry):
        _registry(registry)
    {}

    bool Bootstrap::initialize(const LibraryHandle& libraryHandle, DataContext& context)
    {
        loadConfiguration(libraryHandle, context);
        connectDevices(libraryHandle, context);
        buildPipeline(libraryHandle, context);
        loadExtensions(libraryHandle, context);
        attachEventHandlers(libraryHandle, context);
        attachScrips(libraryHandle, context);

        return true;
    }

    bool Bootstrap::run(DataContext& context)
    {
        _registry.get<ILoopService>([&context](ILoopService* frameService) {
            frameService->startLoop(context);
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

    void Bootstrap::loadConfiguration(const LibraryHandle& libraryHandle, DataContext& context)
    {
        _registry.get<IMultiSinkLogService>(Logger::Config, [this, &context](IMultiSinkLogService* logger){
            logger->initialize(context.configuration.loggingConfiguration.configurationLogger, _registry);
        });

        _registry.get<IConfigurationService>([&context](IConfigurationService* configurationService){
            configurationService->load(context.configuration);
        });

        _registry.get<IMultiSinkLogService, Logger::App>([this, &context](IMultiSinkLogService* logger){
            logger->initialize(context.configuration.loggingConfiguration.mainLogger, _registry);
        });

        _registry.get<ILocalizationService>([&context](ILocalizationService* localizationService){
            localizationService->load(context.configuration, context.localization);
        });
    }

    void connectDeviceGroup(DataContext& context, const auto& devices)
    {
        for(const auto& [key, value] : devices)
        {
            auto deviceRef = value.lock();
            if(deviceRef)
            {
                deviceRef->connect(context);
            }
        }
    }

    void Bootstrap::connectDevices(const LibraryHandle& libraryHandle, DataContext& context)
    {
        _registry.get<
                Service<ILogService, Logger::App>,
                Service<IStandardIODevice>,
                Service<ITerminalDevice>,
                Service<IImageStreamDevice>,
                Service<IFileWatcherDevice>>(
        [&context](
            ILogService* logger,
            IStandardIODevice* ioDevice,
            ITerminalDevice* terminalDevice,
            IImageStreamDevice* imageStreamDevice,
            IFileWatcherDevice* fileWatcherDevice)
        {
            logger->information(fmt::format("Dory Game, {0}.{1}, {2}",
                                            context.configuration.buildInfo.version,
                                            context.configuration.buildInfo.commitSHA,
                                            context.configuration.buildInfo.timestamp));

            ioDevice->connect(context);
            terminalDevice->connect(context);
            fileWatcherDevice->connect(context);
            imageStreamDevice->connect(context);
        });

        _registry.getAll<IWindowSystemDevice, WindowSystem>([&context](const auto& devices) {
            connectDeviceGroup(context, devices);
        });

        _registry.getAll<IGpuDevice, GraphicalSystem>([&context](const auto& devices) {
            connectDeviceGroup(context, devices);
        });
    }

    void Bootstrap::buildPipeline(const LibraryHandle& libraryHandle, DataContext& context)
    {
        auto pipelineRepo = _registry.get<IPipelineRepository>();

        pipelineRepo->addNode(nullId, libraryHandle, std::make_shared<WindowSystemController>(_registry));

        pipelineRepo->addNode(nullId, libraryHandle, [this](auto nodeId, const auto& timeStep, DataContext& context){
            _registry.get<
                    Service<events::window::Bundle::IDispatcher>,
                    Service<events::io::Bundle::IDispatcher>,
                    Service<events::filesystem::Bundle::IDispatcher>,
                    Service<events::scene::Bundle::IDispatcher>>(
                [&context](events::window::Bundle::IDispatcher* windowDispatcher,
                            events::io::Bundle::IDispatcher* ioDispatcher,
                            events::filesystem::Bundle::IDispatcher* fsDispatcher,
                           events::scene::Bundle::IDispatcher* sceneDispatcher)
            {
                windowDispatcher->fireAll(context);
                ioDispatcher->fireAll(context);
                fsDispatcher->fireAll(context);
                sceneDispatcher->fireAll(context);
            });
        });

        generic::model::TimeSpan animationStepTimeAccumulator {};
        constexpr auto fixedDeltaTime = generic::model::TimeSpan{1.f / 30.f};
        const auto animationNodeId = pipelineRepo->addTriggerNode(nullId, libraryHandle, [&animationStepTimeAccumulator, fixedDeltaTime](auto nodeId, const auto& timeStep, DataContext& context)
        {
            constexpr int maxUpdatesPerFrame = 5;
            std::size_t updates { 0 };
            animationStepTimeAccumulator += timeStep;
            context.viewStatesUpdateTimeDelta = {};
            while(animationStepTimeAccumulator >= fixedDeltaTime && updates < maxUpdatesPerFrame)
            {
                updates++;
                animationStepTimeAccumulator -= fixedDeltaTime;
                context.viewStatesUpdateTimeDelta += fixedDeltaTime;
            }

            return updates;
        });

        pipelineRepo->addNode(animationNodeId, libraryHandle, std::make_shared<AccelerationMovementController>(_registry));
        pipelineRepo->addNode(animationNodeId, libraryHandle, std::make_shared<MovementController>(_registry));
        pipelineRepo->addNode(animationNodeId, libraryHandle, std::make_shared<TransformController>(_registry));
        pipelineRepo->addNode(animationNodeId, libraryHandle, [this, fixedDeltaTime](auto nodeId, const auto& timeStep, DataContext& context) {
            if(auto viewService = _registry.get<IViewService>())
            {
                viewService->updateViewsState(context.viewStates);
                context.viewStatesUpdateTime = std::chrono::steady_clock::now();
            }
        });

        pipelineRepo->addNode(nullId, libraryHandle, [this](auto nodeId, const auto& timeStep, DataContext& context) {
            _registry.get<IStandardIODevice>([](IStandardIODevice* ioDevice){
                ioDevice->flush();
            });
        });
    }

    void Bootstrap::loadExtensions(const LibraryHandle& libraryHandle, DataContext& context)
    {
        _registry.get<ILibraryService>([&context](ILibraryService* libraryService) {
            for(const auto& extension : context.configuration.extensions)
            {
                libraryService->load(context, extension);
            }
        });
    }

    std::string threadIdToString(const std::thread::id& id) {
        std::stringstream ss;
        ss << id;
        return ss.str();
    }

    void Bootstrap::attachEventHandlers(const LibraryHandle& libraryHandle, DataContext& context)
    {
        _registry.get<events::application::Bundle::IListener>([this](events::application::Bundle::IListener* listener){
            listener->attach([this](auto& context, const auto& event){ this->onApplicationExit(context, event); });
        });

        _registry.get<events::window::Bundle::IListener>([this](events::window::Bundle::IListener* listener){
            listener->attach([this](auto& context, const events::window::Close& event){ this->onWindowClose(context, event); });
        });

        _registry.get<events::filesystem::Bundle::IListener>([this](events::filesystem::Bundle::IListener* listener){
            listener->attach([this](auto& context, const auto& event){ this->onFilesystemEvent(context, event); });
        });

        _registry.get<ILogService, Logger::App>([](ILogService* logger){
            logger->information("main threadId: " + threadIdToString(std::this_thread::get_id()));
        });
    }

    void Bootstrap::attachScrips(const LibraryHandle& libraryHandle, DataContext& context)
    {
        _registry.get<IScriptService>([this, &libraryHandle](IScriptService* scriptService){

            scriptService->addScript("exit", libraryHandle, [this](DataContext& context, const std::map<std::string, std::any>& arguments){
                _registry.get<
                        Service<ITerminalDevice>,
                        Service<events::application::Bundle::IDispatcher>>(
                [&context](ITerminalDevice* terminalDevice, events::application::Bundle::IDispatcher* applicationDispatcher){
                    terminalDevice->writeLine(fmt::format("-\u001B[31m{0}\u001B[0m-", "exit"));
                    applicationDispatcher->fire(context, events::application::Exit{});
                });
            });

        });
    }

    void Bootstrap::onApplicationExit(DataContext& context, const events::application::Exit& eventData)
    {
        _registry.get<ILoopService>([](ILoopService* frameService) {
            frameService->endLoop();
        });
    }

    void Bootstrap::onWindowClose(DataContext& context, const events::window::Close& eventData)
    {
        _registry.get<IWindowService>([this, &eventData, &context](IWindowService* windowService) {
            windowService->closeWindow(eventData.windowId, eventData.windowSystem);

            if(eventData.windowId == context.mainWindowId)
            {
                auto dispatcher = _registry.get<events::application::Bundle::IDispatcher>();
                if(dispatcher)
                {
                    dispatcher->fire(context, events::application::Exit{});
                }
            }
        });
    }

    void Bootstrap::onFilesystemEvent(DataContext& context, const events::filesystem::FileModified& event)
    {
        auto resolver = _registry.get<IAssetTypeResolver>();
        if(resolver)
        {
            auto assetType = resolver->resolve(context, event.filePath);
            if(assetType)
            {
                _registry.get<IAssetReloadHandler>(*assetType, [&context, &event](IAssetReloadHandler* assetLoader) {
                    assetLoader->reload(context, event.filePath);
                });
            }
        }

    }
}