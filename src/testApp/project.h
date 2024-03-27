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

    public:
        int run()
        {
            attachEventHandlers();

            DataContextType context;

            services.engine.initialize(context);
            services.frameService.startLoop(context);

            return 0;
        }

    private:
        void attachEventHandlers()
        {
            services.engineEventHub.onInitializeEngine().attachHandler(this, &Project::onInitializeEngine);
            services.engineEventHub.onStopEngine().attachHandler(this, &Project::onStopEngine);
            services.consoleEventHub.onKeyPressed().attachHandler(this, &Project::onConsoleKeyPressed);
            services.windowEventHub.onCloseWindow().attachHandler(this, &Project::onCloseWindow);
        }

        void onInitializeEngine(DataContextType& context, const events::InitializeEngineEventData& eventData)
        {
            std::cout << "Starting Engine..." << std::endl;

            services.pipelineManager.configurePipeline(context);

            auto window = services.windowService.createWindow();
            context.mainWindowId = window.id;
            services.viewService.createView(context, window.id, context.outputGroupNodeId);
        }

        void onStopEngine(DataContextType& context, const events::StopEngineEventData& eventData)
        {
            std::cout << "Stopping Engine..." << std::endl;
        }

        void onConsoleKeyPressed(DataContextType& context, events::KeyPressedEventData& eventData)
        {
            if(eventData.keyPressed == 27)
            {
                services.frameService.endLoop();
                std::cout << std::this_thread::get_id() << ": ESC" << std::endl;
            }
            else if(eventData.keyPressed == 119)
            {
                auto window = services.windowService.createWindow();
                services.viewService.createView(context, window.id, context.outputGroupNodeId);
            }
            else if(eventData.keyPressed != 0)
            {
                std::cout << std::this_thread::get_id() << ": key pressed: " << eventData.keyPressed << std::endl;
            }
        }

        void onCloseWindow(DataContextType& context, events::CloseWindowEventData& eventData)
        {
            services.windowService.closeWindow(eventData.windowId);
            services.viewService.destroyView(eventData.windowId);

            if(eventData.windowId == context.mainWindowId)
            {
                services.frameService.endLoop();
            }
        }
    };
}