#pragma once

#include "base/serviceContainer.h"
#include "base/configuration/fileSystemBasedConfiguration.h"
#include "base/domain/events/engineEventHub.h"
#include "base/domain/entity.h"
#include "base/domain/idFactory.h"
#include "base/domain/entityRepository.h"
#include "base/domain/engine.h"
#include "openGL/glfwWindow.h"
#include "base/domain/services/frameService.h"

namespace dory
{
    template<>
    struct ServiceInstantiator<dory::configuration::FileSystemBasedConfiguration>
    {
        template<typename TServiceContainer>
        static decltype(auto) createInstance(TServiceContainer& services)
        {
            return dory::configuration::FileSystemBasedConfiguration{"configuration"};
        }
    };
}


namespace testApp
{
    namespace entity = dory::domain::entity;
    namespace domain = dory::domain;

    template<typename TDataContext>
    struct ServiceDependencies
    {
        using IdType = entity::IdType;
        using ConfigurationServiceType = dory::configuration::FileSystemBasedConfiguration;
        using CameraRepositoryType = domain::EntityRepository2<entity::Camera, IdType>;
        using ViewRepositoryType = domain::EntityRepository2<entity::View, IdType>;
        using WindowRepositoryType = domain::EntityRepository2<dory::openGL::GlfwWindow, IdType>;
        using PipelineRepositoryType = domain::services::PipelineRepository<entity::PipelineNode, IdType>;
        using EngineType = domain::Engine2<TDataContext, PipelineRepositoryType>;
        using FrameServiceType = services::BasicFrameService2<TDataContext, EngineType>;

        using ConfigurationService = dory::Singleton<ConfigurationServiceType>;

        using EngineEventHubDispatcher = dory::Singleton<events::EngineEventHubDispatcher<TDataContext>>;
        using EngineEventHub = dory::Reference<EngineEventHubDispatcher, events::EngineEventHub<TDataContext>>;
        using ConsoleEventHubDispatcher = dory::Singleton<events::SystemConsoleEventHubDispatcher<TDataContext>>;
        using ConsoleEventHub = dory::Reference<ConsoleEventHubDispatcher, events::SystemConsoleEventHub<TDataContext>>;
        using WindowEventHubDispatcher = dory::Singleton<events::WindowEventHubDispatcher<TDataContext>>;
        using WindowEventHub = dory::Reference<WindowEventHubDispatcher, events::WindowEventHub<TDataContext>>;

        using PipelineRepository = dory::Singleton<PipelineRepositoryType, services::IPipelineRepository<PipelineRepositoryType>>;
        using CameraRepository = dory::Singleton<CameraRepositoryType, domain::IEntityRepository<CameraRepositoryType, entity::Camera, IdType>>;
        using ViewRepository = dory::Singleton<ViewRepositoryType, domain::IEntityRepository<ViewRepositoryType, entity::View, IdType>>;
        using WindowRepository = dory::Singleton<WindowRepositoryType, domain::IEntityRepository<WindowRepositoryType, dory::openGL::GlfwWindow, IdType>>;

        using Engine = dory::Singleton<EngineType, domain::IEngine<EngineType, TDataContext>, DependencyList<EngineEventHubDispatcher, PipelineRepository>>;
        using FrameService = dory::Singleton<FrameServiceType, services::IFrameService<FrameServiceType, TDataContext>, DependencyList<Engine>>;

        using ServiceContainerType = dory::ServiceContainer<
                ConfigurationService,
                EngineEventHubDispatcher,
                EngineEventHub,
                ConsoleEventHubDispatcher,
                ConsoleEventHub,
                WindowEventHubDispatcher,
                WindowEventHub,
                PipelineRepository,
                CameraRepository,
                ViewRepository,
                WindowRepository,
                Engine,
                FrameService>;
    };

    class Project2
    {
    private:
        using Services = ServiceDependencies<ProjectDataContext>;

        Services::ServiceContainerType& services;

    public:
        explicit Project2(Services::ServiceContainerType &services):
            services(services)
        {
            attachEventHandlers();
        }

        void run(ProjectDataContext& context)
        {
            services.get<Services::Engine>().initialize(context);
            services.get<Services::FrameService>().startLoop(context);
        }

    private:
        void attachEventHandlers()
        {
            auto& engineEventHub = services.get<Services::EngineEventHub>();
            engineEventHub.onInitializeEngine().attachHandler(this, &Project2::onInitializeEngine);
            engineEventHub.onStopEngine().attachHandler(this, &Project2::onStopEngine);

            auto& consoleEventHub = services.get<Services::ConsoleEventHub>();
            consoleEventHub.onKeyPressed().attachHandler(this, &Project2::onConsoleKeyPressed);

            auto& windowEventHub = services.get<Services::WindowEventHub>();
            windowEventHub.onCloseWindow().attachHandler(this, &Project2::onCloseWindow);
        }

        void onInitializeEngine(ProjectDataContext& context, const events::InitializeEngineEventData& eventData)
        {
            std::cout << "Starting Engine..." << std::endl;

            /*configurePipeline(context);
            context.mainWindowId = newWindow(context);*/
        }

        void onStopEngine(ProjectDataContext& context, const events::StopEngineEventData& eventData)
        {
            std::cout << "Stopping Engine..." << std::endl;
        }

        void onConsoleKeyPressed(ProjectDataContext& context, events::KeyPressedEventData& eventData)
        {
            if(eventData.keyPressed == 27)
            {
                services.get<Services::FrameService>().endLoop();
                std::cout << std::this_thread::get_id() << ": ESC" << std::endl;
            }
            else if(eventData.keyPressed == 119)
            {
                //newWindow(context);
            }
            else if(eventData.keyPressed != 0)
            {
                std::cout << std::this_thread::get_id() << ": key pressed: " << eventData.keyPressed << std::endl;
            }
        }

        void onCloseWindow(ProjectDataContext& context, events::CloseWindowEventData& eventData)
        {

        }
    };
}