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
#include "openGL/viewControllerOpenGL.h"
#include "openGL/services/shaderService.h"
#include "openGL/renderer.h"
#include "project2.h"

namespace dory
{
    namespace entity = dory::domain::entity;

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
        using WindowRepositoryType = domain::IEntityRepository<TWindowRepository, openGL::GlfwWindow, entity::IdType>;
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
}

namespace testApp
{
    namespace domain = dory::domain;
    namespace entity = domain::entity;
    namespace services = domain::services;
    namespace events = domain::events;
    namespace openGL = dory::openGL;
    namespace configuration = dory::configuration;
    namespace win32 = dory::win32;

    template<typename TDataContext>
    struct ServiceDependencies
    {
        using IdType = entity::IdType;
        using ConfigurationServiceType = configuration::FileSystemBasedConfiguration2;
        using CameraRepositoryType = domain::EntityRepository2<entity::Camera, IdType>;
        using ViewRepositoryType = domain::EntityRepository2<entity::View, IdType>;
        using WindowRepositoryType = domain::EntityRepository2<openGL::GlfwWindow, IdType>;
        using PipelineRepositoryType = domain::services::PipelineRepository<entity::PipelineNode, IdType>;
        using EngineType = domain::Engine2<TDataContext, PipelineRepositoryType>;
        using FrameServiceType = services::BasicFrameService2<TDataContext, EngineType>;
        using ControllerInterfaceType = domain::Controller2<TDataContext>;
        using ConsoleControllerType = win32::ConsoleController2<TDataContext>;
        using WindowControllerType = openGL::GlfwWindowController2<TDataContext, WindowRepositoryType>;
        using ConsoleControllerFactoryType = dory::ServiceFactory<ConsoleControllerType, ControllerInterfaceType>;
        using WindowControllerFactoryType = dory::ServiceFactory<WindowControllerType, ControllerInterfaceType>;
        using PipelineManagerType = services::PipelineManager<TDataContext, ConsoleControllerFactoryType, WindowControllerFactoryType, PipelineRepositoryType>;
        using OpenGLShaderServiceType = openGL::services::ShaderService2<ConfigurationServiceType>;
        using RendererType = openGL::Renderer2<openGL::RendererDependencies<OpenGLShaderServiceType>>;
        using RendererFactoryType = RendererType::FactoryType;
        using ViewControllerType = openGL::ViewControllerOpenGL2<openGL::ViewControllerDependencies<TDataContext,
                                                                                                    RendererType,
                                                                                                    ViewRepositoryType,
                                                                                                    WindowRepositoryType,
                                                                                                    RendererFactoryType>>;
        using ViewControllerFactoryType = ViewControllerType::FactoryType;
        using EngineEventDispatcherType = events::EngineEventHubDispatcher<TDataContext>;
        using EngineEventHubType = events::EngineEventHub<TDataContext>;
        using ConsoleEventDispatcherType = events::SystemConsoleEventHubDispatcher<TDataContext>;
        using ConsoleEventHubType = events::SystemConsoleEventHub<TDataContext>;
        using WindowEventHubDispatcherType = events::WindowEventHubDispatcher<TDataContext>;
        using WindowEventHubType = events::WindowEventHub<TDataContext>;
        using ProjectType = testApp::Project2<testApp::ProjectDependencies<TDataContext,
                                                                            EngineType,
                                                                            FrameServiceType,
                                                                            EngineEventHubType,
                                                                            ConsoleEventHubType,
                                                                            WindowEventHubType,
                                                                            PipelineManagerType>>;

        using EngineEventHubDispatcherDep = dory::Singleton<EngineEventDispatcherType>;
        using EngineEventHubDep = dory::Reference<EngineEventHubDispatcherDep, EngineEventHubType>;
        using ConsoleEventHubDispatcherDep = dory::Singleton<ConsoleEventDispatcherType>;
        using ConsoleEventHubDep = dory::Reference<ConsoleEventHubDispatcherDep, ConsoleEventHubType>;
        using WindowEventHubDispatcherDep = dory::Singleton<WindowEventHubDispatcherType>;
        using WindowEventHubDep = dory::Reference<WindowEventHubDispatcherDep, WindowEventHubType>;

        using PipelineRepositoryDep = dory::Singleton<PipelineRepositoryType, services::IPipelineRepository<PipelineRepositoryType>>;
        using CameraRepositoryDep = dory::Singleton<CameraRepositoryType, domain::IEntityRepository<CameraRepositoryType, entity::Camera, IdType>>;
        using ViewRepositoryDep = dory::Singleton<ViewRepositoryType, domain::IEntityRepository<ViewRepositoryType, entity::View, IdType>>;
        using WindowRepositoryDep = dory::Singleton<WindowRepositoryType, domain::IEntityRepository<WindowRepositoryType, dory::openGL::GlfwWindow, IdType>>;

        using EngineDep = dory::Singleton<EngineType, domain::IEngine<EngineType, TDataContext>, dory::DependencyList<EngineEventHubDispatcherDep, PipelineRepositoryDep>>;
        using FrameServiceDep = dory::Singleton<FrameServiceType, services::IFrameService<FrameServiceType, TDataContext>, dory::DependencyList<EngineDep>>;
        using ConfigurationServiceDep = dory::Singleton<ConfigurationServiceType, configuration::IConfiguration<ConfigurationServiceType>>;
        using ShaderServiceDep = dory::Singleton<OpenGLShaderServiceType, openGL::services::IShaderService<OpenGLShaderServiceType>, dory::DependencyList<ConfigurationServiceDep>>;

        using ConsoleControllerFactoryDep = dory::Singleton<ConsoleControllerFactoryType, dory::IServiceFactory<ConsoleControllerFactoryType>, dory::DependencyList<ConsoleEventHubDispatcherDep>>;
        using WindowControllerFactoryDep = dory::Singleton<WindowControllerFactoryType, dory::IServiceFactory<WindowControllerFactoryType>, dory::DependencyList<WindowRepositoryDep, WindowEventHubDispatcherDep>>;
        using RendererFactoryDep = dory::Singleton<RendererFactoryType, dory::IServiceFactory<RendererFactoryType>, dory::DependencyList<ShaderServiceDep>>;
        using ViewControllerFactoryDep = dory::Singleton<ViewControllerFactoryType, dory::IServiceFactory<ViewControllerFactoryType>, dory::DependencyList<RendererFactoryDep, ViewRepositoryDep, WindowRepositoryDep>>;

        using PipelineManagerDep = dory::Singleton<PipelineManagerType, services::IPipelineManager<PipelineManagerType, TDataContext>,
                dory::DependencyList<ConsoleControllerFactoryDep, WindowControllerFactoryDep, PipelineRepositoryDep>>;

        using ProjectDep = dory::Singleton<ProjectType, ProjectType, dory::DependencyList<EngineDep,
                                                                                            FrameServiceDep,
                                                                                            EngineEventHubDep,
                                                                                            ConsoleEventHubDep,
                                                                                            WindowEventHubDep,
                                                                                            PipelineManagerDep>>;

        using ServiceContainerType = dory::ServiceContainer<
                ConfigurationServiceDep,
                ShaderServiceDep,
                EngineEventHubDispatcherDep,
                EngineEventHubDep,
                ConsoleEventHubDispatcherDep,
                ConsoleEventHubDep,
                WindowEventHubDispatcherDep,
                WindowEventHubDep,
                PipelineRepositoryDep,
                CameraRepositoryDep,
                ViewRepositoryDep,
                WindowRepositoryDep,
                EngineDep,
                FrameServiceDep,
                ConsoleControllerFactoryDep,
                WindowControllerFactoryDep,
                RendererFactoryDep,
                ViewControllerFactoryDep,
                PipelineManagerDep,
                ProjectDep>;
    };
}
