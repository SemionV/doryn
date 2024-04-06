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
        }

        void onInitializeEngine(DataContextType& context, const events::InitializeEngineEventData& eventData)
        {
            services.standartIODevice.connect(context);
            services.terminalDevice.connect(context);
            services.terminalDevice.writeLine("Starting Engine...");
            services.terminalDevice.enterCommandMode();

            services.pipelineManager.configurePipeline(context);

            auto supmitInputEvents = [this](auto referenceId, const auto& timeStep, DataContextType& context)
            {
                services.standartIoEventDispatcher.submitInput(context);
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
            services.standartIODevice.disconnect(context);
        }

        void onApplicationExit(DataContextType& context, const events::ApplicationExitEventData& eventData)
        {
            services.terminalDevice.exitCommandMode();
            services.terminalDevice.writeLine("Stopping Engine...");
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
    };
}