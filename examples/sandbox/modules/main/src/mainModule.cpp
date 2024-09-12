#include <dory/sandbox/mainModule.h>

namespace dory::sandbox
{
    int MainModule::run(bootstrap::StartupModuleContext& moduleContext) {
        auto context = DataContextType{};
        auto& configuration = context.configuration;

        //0 level config
        configuration.section.loadFrom.emplace_back("settings.yaml");
        auto& bootLoggerConfig = configuration.loggingConfiguration.configurationLogger;
        bootLoggerConfig.name = "boot";
        bootLoggerConfig.rotationLogger = dory::configuration::RotationLogSink{"logs/boot.log"};
        bootLoggerConfig.stdoutLogger = dory::configuration::StdoutLogSink{};

        registry.services.appConfigurationLogger.initialize(bootLoggerConfig, dory::makeOptionalRef(registry.devices.terminalDevice));
        registry.services.configurationService.load(configuration);
        registry.services.appLogger.initialize(configuration.loggingConfiguration.mainLogger, dory::makeOptionalRef(registry.devices.terminalDevice));

        attachEventHandlers();

        auto mainController = Registry::ServiceTypes::MainControllerType {registry.events.mainControllerDispatcher, registry.repositories.pipelines };

        frameService.startLoop(context, mainController);

        return 0;
    }

    void MainModule::attachEventHandlers()
    {
        registry.services.appLogger.information("attach event handlers");

        registry.events.mainController.attach(this, &MainModule::onInitializeEngine);
        registry.events.mainController.attach(this, &MainModule::onStopEngine);
        registry.events.application.attach(this, &MainModule::onApplicationExit);
        registry.events.window.attach(this, &MainModule::onCloseWindow);
        registry.events.script.attach(this, &MainModule::onRunScript);
    }

    void MainModule::onInitializeEngine(DataContextType& context, const events::mainController::Initialize& eventData)
    {
        auto& configuration = context.configuration;
        auto& localization = context.localization;

        registry.services.appLogger.information(fmt::format("Dory Sandbox, {0}.{1}, {2}",
                                                            configuration.buildInfo.version,
                                                            configuration.buildInfo.commitSHA,
                                                            configuration.buildInfo.timestamp));

        registry.services.localizationService.load(configuration, localization);
        registry.services.appLogger.information(localization.hello);
        registry.services.appLogger.information(localization.goodBye.get("Semion"));
        registry.services.appLogger.information(localization.birthDate.get(11, 03, 1984));

        configuration.userInterface.activeLanguage = "english";
        registry.services.localizationService.load(configuration, localization);
        registry.services.appLogger.information(localization.hello);
        registry.services.appLogger.information(localization.goodBye.get("Semion"));
        registry.services.appLogger.information(localization.birthDate.get(11, 03, 1984));

        registry.services.appLogger.information("on: initialize engine");

        registry.devices.standardIoDevice.connect(context);
        registry.devices.terminalDevice.connect(context);
        registry.devices.terminalDevice.writeLine("Start Engine...");

        registry.devices.terminalDevice.enterCommandMode();

        auto& scriptService = registry.services.scriptService;
        scriptService.addScript("exit", [this](DataContextType& context, const std::map<std::string, std::any>& arguments)
        {
            registry.devices.terminalDevice.writeLine(fmt::format("-\u001B[31m{0}\u001B[0m-", "exit"));
            registry.events.applicationDispatcher.fire(context, events::application::Exit{});
        });

        scriptService.addScript("load-ext", [this](DataContextType& context, const std::map<std::string, std::any>& arguments)
        {
            registry.devices.terminalDevice.writeLine(fmt::format("\u001B[32m{0}\u001B[0m", "load extension"));

            libraryService.unload("test extension");
            auto library = libraryService.load("test extension", "modules/extension");
            if(library)
            {
                auto extension = library->loadModule<ExtensionContext>("extension", extensionContext);
                if(extension)
                {
                    extension->attach(dory::LibraryHandle{ library });
                }
            }
        });

        scriptService.addScript("unload-ext", [this](DataContextType& context, const std::map<std::string, std::any>& arguments)
        {
            registry.devices.terminalDevice.writeLine(fmt::format("\u001B[32m{0}\u001B[0m", "unload extension"));

            libraryService.unload("test extension");
        });

        registry.services.pipelineService.configurePipeline(context);

        auto supmitInputEvents = [this](auto referenceId, const auto& timeStep, DataContextType& context)
        {
            registry.events.standardIoDispatcher.fireAll(context);
        };

        auto flushOutput = [this](auto referenceId, const auto& timeStep, DataContextType& context)
        {
            registry.devices.standardIoDevice.flush();
        };

        dory::domain::repositories::IPipelineRepository<Registry::RepositoryTypes::PipelineRepositoryType, DataContextType>& pipelines = registry.repositories.pipelines;

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
        registry.services.viewService.createView(context, window.id, context.outputGroupNodeId);
    }

    void MainModule::onStopEngine(DataContextType& context, const events::mainController::Stop& eventData)
    {
        registry.devices.terminalDevice.exitCommandMode();
        registry.devices.terminalDevice.writeLine("Stop Engine...");
        registry.devices.terminalDevice.disconnect(context);
        registry.devices.standardIoDevice.disconnect(context);
    }

    void MainModule::onApplicationExit(DataContextType& context, const events::application::Exit& eventData)
    {
        frameService.endLoop();
    }

    void MainModule::onCloseWindow(DataContextType& context, events::window::Close& eventData)
    {
        registry.services.windowService.closeWindow(eventData.windowId);
        registry.services.viewService.destroyView(eventData.windowId);

        if(eventData.windowId == context.mainWindowId)
        {
            registry.events.applicationDispatcher.fire(context, events::application::Exit{});
        }
    }

    void MainModule::onRunScript(DataContextType& context, const events::script::Run& eventData)
    {
        registry.services.scriptService.runScript(context, eventData.scriptKey, eventData.arguments);
    }
}