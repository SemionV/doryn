#pragma once

#include "registry.h"

namespace testApp
{
    namespace events = dory::domain::events;

    struct LogStrings
    {
        const fmt::runtime_format_string<> devicesConnected = fmt::runtime("DeviceLayer connected {0}");
    };

    class Project: dory::Uncopyable
    {
    private:
        ConfigurationType configuration;
        Registry registry;
        ServiceLayer::FrameServiceType frameService;

    public:
        Project(): registry { configuration }
        {}

        int run()
        {
            //0 level config
            configuration.section.loadFrom.emplace_back("settings.yaml");
            auto& bootLoggerConfig = configuration.loggingConfiguration.configurationLogger;
            bootLoggerConfig.name = "boot";
            bootLoggerConfig.rotationLogger = dory::configuration::RotationLogSink{"logs/boot.log"};
            bootLoggerConfig.stdoutLogger = dory::configuration::StdoutLogSink{};

            //init boot logger with 0 level config
            registry.services.appConfigurationLogger.initialize(bootLoggerConfig, dory::makeOptionalRef(registry.devices.terminalDevice));

            //load configuration
            registry.services.configurationService.load(configuration);

            //init main logger with parents config
            registry.services.appLogger.initialize(configuration.loggingConfiguration.mainLogger, dory::makeOptionalRef(registry.devices.terminalDevice));

            registry.services.appConfigurationLogger.information("active language: " + configuration.interface.activeLanguage);

            configuration.interface.activeLanguage = "spanish";
            registry.services.configurationService.save(configuration);

            attachEventHandlers();

            auto context = DataContextType{};
            auto engine = ServiceLayer::EngineType {registry.events.engineDispatcher, registry.repositories.pipelines };

            frameService.startLoop(context, engine);

            return 0;
        }

    private:
        void attachEventHandlers()
        {
            registry.services.appConfigurationLogger.information("attach event handlers");

            registry.events.engine.attach(this, &Project::onInitializeEngine);
            registry.events.engine.attach(this, &Project::onStopEngine);
            registry.events.application.attach(this, &Project::onApplicationExit);
            registry.events.window.attach(this, &Project::onCloseWindow);
            registry.events.script.attach(this, &Project::onRunScript);
        }

        void onInitializeEngine(DataContextType& context, const events::engine::Initialize& eventData)
        {
            registry.services.appConfigurationLogger.information("on: initialize engine");

            registry.devices.standardIoDevice.connect(context);
            registry.devices.terminalDevice.connect(context);
            registry.devices.terminalDevice.writeLine("Start Engine...");
            registry.devices.terminalDevice.enterCommandMode();

            auto logStrings = LogStrings{};
            registry.services.appLogger.information(fmt::format(logStrings.devicesConnected, ":)"));

            registry.services.scriptService.addScript("exit", [this](DataContextType& context, const std::map<std::string, std::any>& arguments)
            {
                registry.devices.terminalDevice.writeLine(fmt::format("-\u001B[31m{0}\u001B[0m-", "exit"));
                registry.events.applicationDispatcher.fire(context, events::application::Exit{});
            });

            registry.managers.pipelineManager.configurePipeline(context);

            auto supmitInputEvents = [this](auto referenceId, const auto& timeStep, DataContextType& context)
            {
                registry.events.standardIoDispatcher.fireAll(context);
            };

            auto flushOutput = [this](auto referenceId, const auto& timeStep, DataContextType& context)
            {
                registry.devices.standardIoDevice.flush();
            };

            dory::domain::repositories::IPipelineRepository<RepositoryLayer::PipelineRepositoryType, DataContextType>& pipelines = registry.repositories.pipelines;

            pipelines.store(dory::domain::entity::PipelineNode<DataContextType> {
                    supmitInputEvents,
                    dory::domain::entity::PipelineNodePriority::Default,
                    context.inputGroupNodeId});

            pipelines.store(dory::domain::entity::PipelineNode<DataContextType> {
                flushOutput,
                dory::domain::entity::PipelineNodePriority::Default,
                context.outputGroupNodeId});

            auto window = registry.services.windowService.createWindow();
            context.mainWindowId = window.id;
            registry.managers.viewManager.createView(context, window.id, context.outputGroupNodeId);
        }

        void onStopEngine(DataContextType& context, const events::engine::Stop& eventData)
        {
            registry.devices.terminalDevice.exitCommandMode();
            registry.devices.terminalDevice.writeLine("Stop Engine...");
            registry.devices.terminalDevice.disconnect(context);
            registry.devices.standardIoDevice.disconnect(context);

            registry.services.appLogger.information("devices disconnected");
        }

        void onApplicationExit(DataContextType& context, const events::application::Exit& eventData)
        {
            frameService.endLoop();
        }

        void onCloseWindow(DataContextType& context, events::window::Close& eventData)
        {
            registry.services.windowService.closeWindow(eventData.windowId);
            registry.managers.viewManager.destroyView(eventData.windowId);

            if(eventData.windowId == context.mainWindowId)
            {
                registry.events.applicationDispatcher.fire(context, events::application::Exit{});
            }
        }

        void onRunScript(DataContextType& context, const events::script::Run& eventData)
        {
            registry.services.scriptService.runScript(context, eventData.scriptKey, eventData.arguments);
        }
    };
}