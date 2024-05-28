#pragma once

#include "registry.h"

namespace testApp
{
    namespace events = dory::domain::events;

    struct LogStrings
    {
        const fmt::runtime_format_string<>& devicesConnected = fmt::runtime("Devices connected {0}");
    };

    class Project: dory::Uncopyable
    {
    private:
        using DataContextType = typename registry::DataContextType;
        registry::ConfigurationType configuration;
        registry::Services services = registry::Services{configuration};
        registry::FrameServiceType frameService;

    public:
        Project(): services(configuration)
        {}

        int run()
        {
            configuration.loggingConfiguration.configurationLogger.rotationLogger = dory::configuration::RotationLogSink{"boot.log"};

            services.logging.appConfigurationLogger.initialize(configuration.loggingConfiguration.configurationLogger,
                                                               dory::makeOptionalRef(services.terminalDevice));

            services.configurationLoader.load(configuration.mainConfigurationFile, configuration);

            attachEventHandlers();

            auto context = DataContextType{};
            auto engine = registry::EngineType { services.events.engineDispatcher, services.pipelineRepository };

            frameService.startLoop(context, engine);

            return 0;
        }

    private:
        void attachEventHandlers()
        {
            services.logging.appConfigurationLogger.information("attach event handlers");

            services.events.engine.attach(this, &Project::onInitializeEngine);
            services.events.engine.attach(this, &Project::onStopEngine);
            services.events.application.attach(this, &Project::onApplicationExit);
            services.events.window.attach(this, &Project::onCloseWindow);
            services.events.script.attach(this, &Project::onRunScript);
        }

        void onInitializeEngine(DataContextType& context, const events::engine::Initialize& eventData)
        {
            services.logging.appConfigurationLogger.information("on: initialize engine");

            services.standartIODevice.connect(context);
            services.terminalDevice.connect(context);
            services.terminalDevice.writeLine("Start Engine...");
            services.terminalDevice.enterCommandMode();

            auto logStrings = LogStrings{};
            services.logging.appLogger.information(fmt::format(logStrings.devicesConnected, "!"));

            services.terminalDevice.exitCommandMode();
            auto loggerConfiguration = dory::configuration::Logger{"multi-logger"};
            loggerConfiguration.rotationLogger = dory::configuration::RotationLogSink{"logs/multi.log"};
            loggerConfiguration.stdoutLogger = dory::configuration::StdoutLogSink{};
            auto multiLogger = dory::domain::services::MultiSinkLogService{};
            multiLogger.initialize(loggerConfiguration, dory::makeOptionalRef(services.terminalDevice));
            multiLogger.information(fmt::format(logStrings.devicesConnected, "!!"));
            multiLogger.information(fmt::format(logStrings.devicesConnected, "!!!"));

            auto loggerConfiguration2 = dory::configuration::Logger{"multi-logger-2"};
            loggerConfiguration2.stdoutLogger = dory::configuration::StdoutLogSink{};
            multiLogger.initialize(loggerConfiguration2, dory::OptionalReference<testApp::registry::TerminalDeviceType>{});
            multiLogger.information(fmt::format(logStrings.devicesConnected, "-!!"));
            services.terminalDevice.enterCommandMode();

            services.scriptService.addScript("exit", [this](DataContextType& context, const std::map<std::string, std::any>& arguments)
            {
                services.terminalDevice.writeLine(fmt::format("-\u001B[31m{0}\u001B[0m-", "exit"));
                services.events.applicationDispatcher.fire(context, events::application::Exit{});
            });

            services.pipelineManager.configurePipeline(context);

            auto supmitInputEvents = [this](auto referenceId, const auto& timeStep, DataContextType& context)
            {
                services.events.standartIoDispatcher.fireAll(context);
            };

            auto flushOutput = [this](auto referenceId, const auto& timeStep, DataContextType& context)
            {
                services.standartIODevice.flush();
            };

            registry::services::IPipelineRepository<registry::PipelineRepositoryType, DataContextType>& pipelineRepository = services.pipelineRepository;

            pipelineRepository.store(dory::domain::entity::PipelineNode<DataContextType> {
                    supmitInputEvents,
                    dory::domain::entity::PipelineNodePriority::Default,
                    context.inputGroupNodeId});

            pipelineRepository.store(dory::domain::entity::PipelineNode<DataContextType> {
                flushOutput,
                dory::domain::entity::PipelineNodePriority::Default,
                context.outputGroupNodeId});

            auto window = services.windowService.createWindow();
            context.mainWindowId = window.id;
            services.viewService.createView(context, window.id, context.outputGroupNodeId);
        }

        void onStopEngine(DataContextType& context, const events::engine::Stop& eventData)
        {
            services.terminalDevice.exitCommandMode();
            services.terminalDevice.writeLine("Stop Engine...");
            services.terminalDevice.disconnect(context);
            services.standartIODevice.disconnect(context);

            services.logging.appLogger.information("devices disconnected");
        }

        void onApplicationExit(DataContextType& context, const events::application::Exit& eventData)
        {
            frameService.endLoop();
        }

        void onCloseWindow(DataContextType& context, events::window::Close& eventData)
        {
            services.windowService.closeWindow(eventData.windowId);
            services.viewService.destroyView(eventData.windowId);

            if(eventData.windowId == context.mainWindowId)
            {
                services.events.applicationDispatcher.fire(context, events::application::Exit{});
            }
        }

        void onRunScript(DataContextType& context, const events::script::Run& eventData)
        {
            services.scriptService.runScript(context, eventData.scriptKey, eventData.arguments);
        }
    };
}