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
            services.engineEventHub.attach(this, &Project::onInitializeEngine);
            services.engineEventHub.attach(this, &Project::onStopEngine);
            services.applicationEventHub.attach(this, &Project::onApplicationExit);
            services.windowEventHub.attach(this, &Project::onCloseWindow);
            services.scriptEventHub.attach(this, &Project::onRunScript);
        }

        void onInitializeEngine(DataContextType& context, const events::engine::Initialize& eventData)
        {
            services.standartIODevice.connect(context);
            services.terminalDevice.connect(context);
            services.terminalDevice.writeLine("Start Engine...");
            services.terminalDevice.enterCommandMode();

            services.scriptService.addScript("exit", [this](DataContextType& context, const std::map<std::string, std::any>& arguments)
            {
                services.terminalDevice.writeLine("-\u001B[31mexit\u001B[0m-");
                services.applicationEventDispatcher.fire(context, events::application::Exit{});
            });

            services.pipelineManager.configurePipeline(context);

            auto supmitInputEvents = [this](auto referenceId, const auto& timeStep, DataContextType& context)
            {
                services.standartIoEventDispatcher.fireAll(context);
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
                services.applicationEventDispatcher.fire(context, events::application::Exit{});
            }
        }

        void onRunScript(DataContextType& context, const events::script::Run& eventData)
        {
            services.scriptService.runScript(context, eventData.scriptKey, eventData.arguments);
        }
    };
}