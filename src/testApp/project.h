#pragma once

#include "registry.h"

namespace testApp
{
    namespace events = dory::domain::events;

    class Project: dory::Uncopyable
    {
    private:
        using DataContextType = typename registry::DataContextType;
        registry::ServicesLocal services;
        registry::FrameServiceType frameService;

    public:
        int run()
        {
            attachEventHandlers();

            auto context = DataContextType{};
            auto engine = registry::EngineType { services.engineEventDispatcher, services.pipelineRepository };

            frameService.startLoop(context, engine);

            return 0;
        }

    private:
        void attachEventHandlers()
        {
            services.engineEventHub.onInitializeEngine().attachHandler(this, &Project::onInitializeEngine);
            services.engineEventHub.onStopEngine().attachHandler(this, &Project::onStopEngine);
            services.applicationEventHub.onExit().attachHandler(this, &Project::onApplicationExit);
            services.windowEventHub.onCloseWindow().attachHandler(this, &Project::onCloseWindow);
            services.scriptEventHub.onRunScript().attachHandler(this, &Project::onRunScript);
            services.applicationNotificationsEventHub.onNotification().attachHandler(this, &Project::onApplicationNotification);
            services.applicationNotificationsEventHub.onProblem().attachHandler(this, &Project::onApplicationProblem);
            services.applicationNotificationsEventHub.onDisaster().attachHandler(this, &Project::onApplicationDisaster);
        }

        void onInitializeEngine(DataContextType& context, const events::InitializeEngineEventData& eventData)
        {
            services.standartIODevice.connect(context);
            services.terminalDevice.connect(context);
            services.terminalDevice.writeLine("Start Engine...");
            services.terminalDevice.enterCommandMode();

            services.scriptService.addScript("exit", [this](DataContextType& context, const std::map<std::string, std::any>& arguments)
            {
                services.terminalDevice.writeLine("-\u001B[31mexit\u001B[0m-");
                services.applicationEventDispatcher.fire(context, events::ApplicationExitEventData{});
            });

            services.pipelineManager.configurePipeline(context);

            auto supmitInputEvents = [this](auto referenceId, const auto& timeStep, DataContextType& context)
            {
                services.standartIoEventDispatcher.submit(context);
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

        void onStopEngine(DataContextType& context, const events::StopEngineEventData& eventData)
        {
            services.terminalDevice.exitCommandMode();
            services.terminalDevice.writeLine("Stop Engine...");
            services.terminalDevice.disconnect(context);
            services.standartIODevice.disconnect(context);
        }

        void onApplicationExit(DataContextType& context, const events::ApplicationExitEventData& eventData)
        {
            frameService.endLoop();
        }

        void onCloseWindow(DataContextType& context, events::CloseWindowEventData& eventData)
        {
            services.windowService.closeWindow(eventData.windowId);
            services.viewService.destroyView(eventData.windowId);

            if(eventData.windowId == context.mainWindowId)
            {
                services.applicationEventDispatcher.fire(context, events::ApplicationExitEventData{});
            }
        }

        void onRunScript(DataContextType& context, const events::script::RunScriptEventData& eventData)
        {
            services.scriptService.runScript(context, eventData.scriptKey, eventData.arguments);
        }

        void onApplicationNotification(DataContextType& context, const events::notification::application::Notification& eventData)
        {
            services.terminalDevice.writeLine(eventData.message);
        }

        void onApplicationProblem(DataContextType& context, const events::notification::application::Problem& eventData)
        {
            auto message = "\u001B[34m" + eventData.message + "\u001B[0m";
            services.terminalDevice.writeLine(message);
        }

        void onApplicationDisaster(DataContextType& context, const events::notification::application::Disaster& eventData)
        {
            auto message = "\u001B[31m" + eventData.message + "\u001B[0m";
            services.terminalDevice.writeLine(message);
        }
    };
}