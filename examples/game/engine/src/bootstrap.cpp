#include "dory/game/bootstrap.h"
#include <dory/core/controllers/windowSystemController.h>
#include <dory/core/controllers/viewController.h>

namespace dory::game
{
    Bootstrap::Bootstrap(core::Registry &registry):
        _registry(registry)
    {}

    bool Bootstrap::initialize(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context)
    {
        loadConfiguration(libraryHandle, context);
        connectDevices(libraryHandle, context);
        buildPipeline(libraryHandle, context);
        attachEventHandlers(libraryHandle, context);
        attachScrips(libraryHandle, context);

        return true;
    }

    bool Bootstrap::run(core::resources::DataContext& context)
    {
        _registry.get<dory::core::services::IFrameService>([&context](dory::core::services::IFrameService* frameService) {
            frameService->startLoop(context);
        });

        return true;
    }

    void Bootstrap::cleanup(core::resources::DataContext &context)
    {
        _registry.get<
                generic::registry::Service<dory::core::services::ILogService, core::resources::Logger::App>,
                generic::registry::Service<core::devices::ITerminalDevice>,
                generic::registry::Service<core::devices::IStandardIODevice>>
                ([&context](dory::core::services::ILogService* logger,
                    core::devices::ITerminalDevice* terminalDevice,
                    core::devices::IStandardIODevice* ioDevice)
                {
                    terminalDevice->exitCommandMode();
                    logger->information(std::string_view {"Cleanup..."});
                    terminalDevice->disconnect(context);
                    ioDevice->disconnect(context);
                });

        _registry.getAll<core::devices::IWindowSystemDevice, core::resources::WindowSystem>([&context](const auto& devices) {
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

    void Bootstrap::loadConfiguration(const generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context)
    {
        _registry.get<dory::core::services::IMultiSinkLogService>(dory::core::resources::Logger::Config, [this, &context](dory::core::services::IMultiSinkLogService* logger){
            logger->initialize(context.configuration.loggingConfiguration.configurationLogger, _registry);
        });

        _registry.get<dory::core::services::IConfigurationService>([&context](dory::core::services::IConfigurationService* configurationService){
            configurationService->load(context.configuration);
        });

        _registry.get<dory::core::services::IMultiSinkLogService, dory::core::resources::Logger::App>([this, &context](dory::core::services::IMultiSinkLogService* logger){
            logger->initialize(context.configuration.loggingConfiguration.mainLogger, _registry);
        });

        _registry.get<dory::core::services::ILocalizationService>([&context](dory::core::services::ILocalizationService* localizationService){
            localizationService->load(context.configuration, context.localization);
        });
    }

    void Bootstrap::connectDevices(const generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context)
    {
        _registry.get<
                dory::generic::registry::Service<dory::core::services::ILogService, dory::core::resources::Logger::App>,
                dory::generic::registry::Service<dory::core::devices::IStandardIODevice>,
                dory::generic::registry::Service<dory::core::devices::ITerminalDevice>>(
        [&context](
        dory::core::services::ILogService* logger,
        dory::core::devices::IStandardIODevice* ioDevice,
        dory::core::devices::ITerminalDevice* terminalDevice)
        {
            logger->information(fmt::format("Dory Game, {0}.{1}, {2}",
                                            context.configuration.buildInfo.version,
                                            context.configuration.buildInfo.commitSHA,
                                            context.configuration.buildInfo.timestamp));

            ioDevice->connect(context);
            terminalDevice->connect(context);
            terminalDevice->enterCommandMode();
        });

        _registry.getAll<core::devices::IFileWatcherDevice, core::resources::FileWatchSystem>([&context](const auto& devices) {
            for(const auto& [key, value] : devices)
            {
                auto deviceRef = value.lock();
                if(deviceRef)
                {
                    deviceRef->connect(context);
                }
            }
        });

        _registry.getAll<core::devices::IWindowSystemDevice, core::resources::WindowSystem>([&context](const auto& devices) {
            for(const auto& [key, value] : devices)
            {
                auto deviceRef = value.lock();
                if(deviceRef)
                {
                    deviceRef->connect(context);
                }
            }
        });
    }

    void Bootstrap::buildPipeline(const generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context)
    {
        _registry.get<dory::core::repositories::IPipelineRepository>([this, &libraryHandle, &context](dory::core::repositories::IPipelineRepository* pipelineRepository){

            auto inputGroup = core::resources::entity::PipelineNode{};
            auto outputGroup = core::resources::entity::PipelineNode{};

            auto inputGroupId = context.inputGroupNodeId = pipelineRepository->addNode(inputGroup);
            auto outputGroupId = context.outputGroupNodeId = pipelineRepository->addNode(outputGroup);

            auto submitInputEvents = [this](auto referenceId, const auto& timeStep, dory::core::resources::DataContext& context){
                _registry.get<dory::core::events::io::Bundle::IDispatcher>([&context](dory::core::events::io::Bundle::IDispatcher* dispatcher){
                    dispatcher->fireAll(context);
                });
            };
            auto updateHandle = dory::generic::extension::ResourceHandle<dory::core::resources::entity::PipelineNode::UpdateFunctionType>{ libraryHandle, submitInputEvents };
            auto node = dory::core::resources::entity::PipelineNode(updateHandle, inputGroupId);
            pipelineRepository->addNode(node);

            auto flushOutput = [this](auto referenceId, const auto& timeStep, dory::core::resources::DataContext& context){
                _registry.get<dory::core::devices::IStandardIODevice>([](dory::core::devices::IStandardIODevice* ioDevice){
                    ioDevice->flush();
                });
            };
            updateHandle = dory::generic::extension::ResourceHandle<dory::core::resources::entity::PipelineNode::UpdateFunctionType>{ libraryHandle, flushOutput };
            node = dory::core::resources::entity::PipelineNode(updateHandle, outputGroupId);
            pipelineRepository->addNode(node);

            auto windowSystemController = std::make_shared<core::controllers::WindowSystemController>(_registry);
            auto controllerHandle = generic::extension::ResourceHandle<core::resources::entity::PipelineNode::ControllerPointerType>{ libraryHandle, windowSystemController };
            pipelineRepository->addNode(core::resources::entity::PipelineNode{controllerHandle, inputGroupId});

            auto viewController = std::make_shared<core::controllers::ViewController>(_registry);
            controllerHandle = generic::extension::ResourceHandle<core::resources::entity::PipelineNode::ControllerPointerType>{ libraryHandle, viewController };
            pipelineRepository->addNode(core::resources::entity::PipelineNode{controllerHandle, outputGroupId});
        });
    }

    void Bootstrap::attachEventHandlers(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context)
    {
        _registry.get<core::events::application::Bundle::IListener>([this](core::events::application::Bundle::IListener* listener){
            listener->attach([this](auto& context, const auto& event){ this->onApplicationExit(context, event); });
        });

        _registry.get<core::events::window::Bundle::IListener>([this](core::events::window::Bundle::IListener* listener){
            listener->attach([this](auto& context, const auto& event){ this->onWindowClose(context, event); });
        });
    }

    void Bootstrap::attachScrips(const generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context)
    {
        _registry.get<dory::core::services::IScriptService>([this, &libraryHandle](dory::core::services::IScriptService* scriptService){

            scriptService->addScript("exit", libraryHandle, [this](core::resources::DataContext& context, const std::map<std::string, std::any>& arguments){
                _registry.get<
                        generic::registry::Service<core::devices::ITerminalDevice>,
                        generic::registry::Service<core::events::application::Bundle::IDispatcher>>(
                [&context](core::devices::ITerminalDevice* terminalDevice, core::events::application::Bundle::IDispatcher* applicationDispatcher){
                    terminalDevice->writeLine(fmt::format("-\u001B[31m{0}\u001B[0m-", "exit"));
                    applicationDispatcher->fire(context, core::events::application::Exit{});
                });
            });

        });
    }

    void Bootstrap::onApplicationExit(core::resources::DataContext& context, const core::events::application::Exit& eventData)
    {
        _registry.get<dory::core::services::IFrameService>([](dory::core::services::IFrameService* frameService) {
            frameService->endLoop();
        });
    }

    void Bootstrap::onWindowClose(core::resources::DataContext& context, const core::events::window::Close& eventData)
    {
        _registry.get<core::services::IWindowService>(eventData.windowSystem, [this, &eventData, &context](core::services::IWindowService* windowService) {
            windowService->closeWindow(eventData.windowId);

            if(eventData.windowId == context.mainWindowId)
            {
                auto dispatcher = _registry.get<core::events::application::Bundle::IDispatcher>();
                if(dispatcher)
                {
                    dispatcher->fire(context, core::events::application::Exit{});
                }
            }
        });
    }
}