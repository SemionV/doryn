#include <dory/sandbox/mainModule.h>

namespace dory::sandbox
{
    MainModule::MainModule():
            registry { configuration }
    {}

    int MainModule::run(bootstrap::StartupModuleContext& moduleContext) {
        //0 level config
        configuration.section.loadFrom.emplace_back("settings.yaml");
        auto& bootLoggerConfig = configuration.loggingConfiguration.configurationLogger;
        bootLoggerConfig.name = "boot";
        bootLoggerConfig.rotationLogger = dory::configuration::RotationLogSink{"logs/boot.log"};
        bootLoggerConfig.stdoutLogger = dory::configuration::StdoutLogSink{};

        registry.services.appConfigurationLogger.initialize(bootLoggerConfig, dory::makeOptionalRef(registry.devices.terminalDevice));
        registry.services.configurationService.load(configuration);
        registry.services.appLogger.initialize(configuration.loggingConfiguration.mainLogger, dory::makeOptionalRef(registry.devices.terminalDevice));

        Registry::LocalizationType localization;
        registry.services.localizationService.load(configuration, localization);
        registry.services.appLogger.information(localization.hello);
        registry.services.appLogger.information(localization.goodBye.get("Semion"));
        registry.services.appLogger.information(localization.birthDate.get(11, 03, 1984));

        configuration.userInterface.activeLanguage = "english";
        registry.services.localizationService.load(configuration, localization);
        registry.services.appLogger.information(localization.hello);
        registry.services.appLogger.information(localization.goodBye.get("Semion"));
        registry.services.appLogger.information(localization.birthDate.get(11, 03, 1984));

        attachEventHandlers();

        auto context = DataContextType{};
        auto engine = Registry::ServiceTypes::EngineType { registry.events.engineDispatcher, registry.repositories.pipelines };

        frameService.startLoop(context, engine);

        return 0;
    }

    void MainModule::attachEventHandlers()
    {
        registry.services.appLogger.information("attach event handlers");

        registry.events.engine.attach(this, &MainModule::onInitializeEngine);
        registry.events.engine.attach(this, &MainModule::onStopEngine);
        registry.events.application.attach(this, &MainModule::onApplicationExit);
        registry.events.window.attach(this, &MainModule::onCloseWindow);
        registry.events.script.attach(this, &MainModule::onRunScript);
    }

    void MainModule::onInitializeEngine(DataContextType& context, const events::engine::Initialize& eventData)
    {
        registry.services.appLogger.information("on: initialize engine");

        registry.devices.standardIoDevice.connect(context);
        registry.devices.terminalDevice.connect(context);
        registry.devices.terminalDevice.writeLine("Start Engine...");

        auto library = libraryService.load("test extension", "modules/extension");
        if(library)
        {
            auto extension = library->loadModule<ExtensionContext>("extension", extensionContext);
            if(extension)
            {
                extension->attach(dory::LibraryHandle{ library });
            }
        }

        registry.devices.terminalDevice.enterCommandMode();

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
        registry.managers.viewManager.createView(context, window.id, context.outputGroupNodeId);
    }

    void MainModule::onStopEngine(DataContextType& context, const events::engine::Stop& eventData)
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
        registry.managers.viewManager.destroyView(eventData.windowId);

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