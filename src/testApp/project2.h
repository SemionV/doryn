#pragma once

#include "base/serviceContainer.h"
#include "base/domain/configuration.h"
#include "base/domain/events/engineEventHub.h"
#include "base/domain/entity.h"
#include "base/domain/idFactory.h"
#include "base/domain/entityRepository.h"
#include "base/domain/engine.h"
#include "base/domain/services/frameService.h"
#include "win32/consoleController.h"
#include "openGL/glfwWindow.h"
#include "openGL/glfwWindowController.h"
#include "openGL/services/shaderService.h"

namespace dory
{
    namespace entity = dory::domain::entity;
    namespace services = dory::domain::services;

    template<>
    struct ServiceInstantiator<dory::configuration::FileSystemBasedConfiguration2>
    {
        template<typename TServiceContainer>
        static decltype(auto) createInstance(TServiceContainer& services)
        {
            return dory::configuration::FileSystemBasedConfiguration2{"configuration"};
        }
    };

    template<typename TDataContext, typename TControllerInterface>
    class ServiceFactory<dory::win32::ConsoleController2<TDataContext>, TControllerInterface>:
            public IServiceFactory<ServiceFactory<dory::win32::ConsoleController2<TDataContext>, TControllerInterface>>
    {
    private:
        using ConsoleEventDispatcherType = dory::domain::events::SystemConsoleEventHubDispatcher<TDataContext>;
        ConsoleEventDispatcherType& consoleEventDispatcher;

    public:
        explicit ServiceFactory(ConsoleEventDispatcherType& consoleEventDispatcher):
                consoleEventDispatcher(consoleEventDispatcher)
        {}

        std::shared_ptr<TControllerInterface> createInstanceImpl()
        {
            return std::static_pointer_cast<TControllerInterface>(std::make_shared<dory::win32::ConsoleController2<TDataContext>>(consoleEventDispatcher));
        }
    };

    template<typename TDataContext, typename TControllerInterface, typename TWindowRepository>
    class ServiceFactory<dory::openGL::GlfwWindowController2<TDataContext, TWindowRepository>, TControllerInterface>:
            public IServiceFactory<ServiceFactory<dory::openGL::GlfwWindowController2<TDataContext, TWindowRepository>, TControllerInterface>>
    {
    private:
        using WindowRepositoryType = domain::IEntityRepository<TWindowRepository, openGL::GlfwWindow, domain::entity::IdType>;
        WindowRepositoryType& windowRepository;

        using WindowEventHubType = domain::events::WindowEventHubDispatcher<TDataContext>;
        WindowEventHubType& windowEventHubDispatcher;

    public:
        explicit ServiceFactory(WindowRepositoryType& windowRepository, WindowEventHubType& windowEventHubDispatcher):
                windowRepository(windowRepository),
                windowEventHubDispatcher(windowEventHubDispatcher)
        {}

        std::shared_ptr<TControllerInterface> createInstanceImpl()
        {
            return std::static_pointer_cast<TControllerInterface>(
                    std::make_shared<dory::openGL::GlfwWindowController2<TDataContext, TWindowRepository>>(windowRepository, windowEventHubDispatcher));
        }
    };

    template<typename TRendererInterface, typename TShaderService>
    class ServiceFactory<dory::openGL::Renderer2<TShaderService>, TRendererInterface>:
            public IServiceFactory<ServiceFactory<dory::openGL::Renderer2<TShaderService>, TRendererInterface>>
    {
    private:
        using ShaderServiceType = dory::openGL::services::IShaderService<TShaderService>;
        ShaderServiceType& shaderService;

    public:
        explicit ServiceFactory(ShaderServiceType& shaderService):
                shaderService(shaderService)
        {}

        decltype(auto) createInstanceImpl()
        {
            return static_cast<TRendererInterface>(dory::openGL::Renderer2<TShaderService>{ shaderService });
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
        using ConfigurationServiceType = dory::configuration::FileSystemBasedConfiguration2;
        using CameraRepositoryType = domain::EntityRepository2<entity::Camera, IdType>;
        using ViewRepositoryType = domain::EntityRepository2<entity::View, IdType>;
        using WindowRepositoryType = domain::EntityRepository2<dory::openGL::GlfwWindow, IdType>;
        using PipelineRepositoryType = domain::services::PipelineRepository<entity::PipelineNode, IdType>;
        using EngineType = domain::Engine2<TDataContext, PipelineRepositoryType>;
        using FrameServiceType = services::BasicFrameService2<TDataContext, EngineType>;
        using ControllerInterfaceType = domain::Controller2<TDataContext>;
        using ConsoleControllerType = dory::win32::ConsoleController2<TDataContext>;
        using WindowControllerType = dory::openGL::GlfwWindowController2<TDataContext, WindowRepositoryType>;
        using ConsoleControllerFactoryType = dory::ServiceFactory<ConsoleControllerType, ControllerInterfaceType>;
        using WindowControllerFactoryType = dory::ServiceFactory<WindowControllerType, ControllerInterfaceType>;
        using PipelineManagerType = services::PipelineManager<TDataContext, ConsoleControllerFactoryType, WindowControllerFactoryType, PipelineRepositoryType>;
        using OpenGLShaderServiceType = dory::openGL::services::ShaderService2<ConfigurationServiceType>;
        using RendererType = dory::openGL::Renderer2<openGL::RendererDependencies<OpenGLShaderServiceType>>;
        using RendererFactoryType = RendererType::FactoryType;
        using ViewControllerType = dory::openGL::ViewControllerOpenGL2<
                openGL::ViewControllerDependencies<TDataContext, RendererType, ViewRepositoryType, WindowRepositoryType, RendererFactoryType>>;
        using ViewControllerFactoryType = ViewControllerType::FactoryType;

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
        using ConfigurationService = dory::Singleton<ConfigurationServiceType, dory::configuration::IConfiguration<ConfigurationServiceType>>;
        using ShaderService = dory::Singleton<OpenGLShaderServiceType, dory::openGL::services::IShaderService<OpenGLShaderServiceType>, DependencyList<ConfigurationService>>;

        using ConsoleControllerFactory = dory::Singleton<ConsoleControllerFactoryType, dory::IServiceFactory<ConsoleControllerFactoryType>, DependencyList<ConsoleEventHubDispatcher>>;
        using WindowControllerFactory = dory::Singleton<WindowControllerFactoryType, dory::IServiceFactory<WindowControllerFactoryType>, DependencyList<WindowRepository, WindowEventHubDispatcher>>;
        using RendererFactoryDep = dory::Singleton<RendererFactoryType, dory::IServiceFactory<RendererFactoryType>, DependencyList<ShaderService>>;
        using ViewControllerFactory = dory::Singleton<ViewControllerFactoryType, dory::IServiceFactory<ViewControllerFactoryType>, DependencyList<RendererFactoryDep, ViewRepository, WindowRepository>>;

        using PipelineManager = dory::Singleton<PipelineManagerType, services::IPipelineManager<PipelineManagerType, TDataContext>,
            DependencyList<ConsoleControllerFactory, WindowControllerFactory, PipelineRepository>>;

        using ServiceContainerType = dory::ServiceContainer<
                ConfigurationService,
                ShaderService,
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
                FrameService,
                ConsoleControllerFactory,
                WindowControllerFactory,
                RendererFactoryDep,
                ViewControllerFactory,
                PipelineManager>;
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

            services.get<Services::PipelineManager>().configurePipeline(context);
            //context.mainWindowId = newWindow(context);
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