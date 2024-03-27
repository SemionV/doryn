#pragma once

#include "base/typeComponents.h"
#include "base/domain/events/engineEventHub.h"
#include "base/domain/engine.h"
#include "base/domain/services/frameService.h"
#include "base/domain/services/viewService.h"

namespace testApp
{
    namespace domain = dory::domain;
    namespace entity = domain::entity;
    namespace services = domain::services;
    namespace events = domain::events;
    namespace openGL = dory::openGL;
    namespace configuration = dory::configuration;
    namespace win32 = dory::win32;

    template<typename TDataContext,
            typename TEngine,
            typename TFrameService,
            typename TEngineEventHub,
            typename TConsoleEventHub,
            typename TWindowEventHub,
            typename TPipelineManager,
            typename TWindowService,
            typename TViewService>
    struct ProjectDependencies
    {
        using DataContextType = TDataContext;
        using EngineType = TEngine;
        using FrameServiceType = TFrameService;
        using EngineEventHubType = TEngineEventHub;
        using ConsoleEventHubType = TConsoleEventHub;
        using WindowEventHubType = TWindowEventHub;
        using PipelineManagerType = TPipelineManager;
        using WindowServiceType = TWindowService;
        using ViewServiceType = TViewService;
    };

    template<typename T>
    requires(dory::is_instance_v<T, ProjectDependencies>)
    class Project2: dory::Uncopyable
    {
    private:
        using DataContextType = typename T::DataContextType;

        using EngineType = domain::IEngine<typename T::EngineType, DataContextType>;
        EngineType& engine;

        using FrameServiceType = services::IFrameService<typename T::FrameServiceType, DataContextType>;
        FrameServiceType& frameService;

        using EngineEventHubType = events::EngineEventHub<DataContextType>;
        EngineEventHubType& engineEventHub;

        using ConsoleEventHubType = events::SystemConsoleEventHub<DataContextType>;
        ConsoleEventHubType& consoleEventHub;

        using WindowEventHubType = events::WindowEventHub<DataContextType>;
        WindowEventHubType& windowEventHub;

        using PipelineManagerType = services::IPipelineManager<typename T::PipelineManagerType, DataContextType>;
        PipelineManagerType& pipelineManager;

        using WindowServiceType = services::IWindowService<typename T::WindowServiceType>;
        WindowServiceType& windowService;

        using ViewServiceType = services::IViewService<typename T::ViewServiceType, DataContextType>;
        ViewServiceType& viewService;

    public:
        explicit Project2(EngineType& engine,
                          FrameServiceType& frameService,
                          EngineEventHubType& engineEventHub,
                          ConsoleEventHubType& consoleEventHub,
                          WindowEventHubType& windowEventHub,
                          PipelineManagerType& pipelineManager,
                          WindowServiceType& windowService,
                          ViewServiceType& viewService):
            engine(engine),
            frameService(frameService),
            engineEventHub(engineEventHub),
            consoleEventHub(consoleEventHub),
            windowEventHub(windowEventHub),
            pipelineManager(pipelineManager),
            windowService(windowService),
            viewService(viewService)
        {
            attachEventHandlers();
        }

        void run(DataContextType& context)
        {
            engine.initialize(context);
            frameService.startLoop(context);
        }

    private:
        void attachEventHandlers()
        {
            engineEventHub.onInitializeEngine().attachHandler(this, &Project2::onInitializeEngine);
            engineEventHub.onStopEngine().attachHandler(this, &Project2::onStopEngine);
            consoleEventHub.onKeyPressed().attachHandler(this, &Project2::onConsoleKeyPressed);
            windowEventHub.onCloseWindow().attachHandler(this, &Project2::onCloseWindow);
        }

        void onInitializeEngine(DataContextType& context, const events::InitializeEngineEventData& eventData)
        {
            std::cout << "Starting Engine..." << std::endl;

            pipelineManager.configurePipeline(context);

            auto window = windowService.createWindow();
            context.mainWindowId = window.id;
            viewService.createView(context, window.id, context.outputGroupNodeId);
        }

        void onStopEngine(DataContextType& context, const events::StopEngineEventData& eventData)
        {
            std::cout << "Stopping Engine..." << std::endl;
        }

        void onConsoleKeyPressed(DataContextType& context, events::KeyPressedEventData& eventData)
        {
            if(eventData.keyPressed == 27)
            {
                frameService.endLoop();
                std::cout << std::this_thread::get_id() << ": ESC" << std::endl;
            }
            else if(eventData.keyPressed == 119)
            {
                auto window = windowService.createWindow();
                viewService.createView(context, window.id, context.outputGroupNodeId);
            }
            else if(eventData.keyPressed != 0)
            {
                std::cout << std::this_thread::get_id() << ": key pressed: " << eventData.keyPressed << std::endl;
            }
        }

        void onCloseWindow(DataContextType& context, events::CloseWindowEventData& eventData)
        {
            windowService.closeWindow(eventData.windowId);
            viewService.destroyView(eventData.windowId);

            if(eventData.windowId == context.mainWindowId)
            {
                frameService.endLoop();
            }
        }
    };
}