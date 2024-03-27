#pragma once

#include <utility>
#include "base/serviceContainer.h"
#include "base/domain/configuration.h"
#include "base/domain/events/engineEventHub.h"
#include "base/domain/entity.h"
#include "base/domain/idFactory.h"
#include "base/domain/entityRepository.h"
#include "base/domain/engine.h"
#include "base/domain/services/frameService.h"
#include "base/domain/services/viewService.h"
#include "win32/consoleController.h"
#include "openGL/glfwWindow.h"
#include "openGL/glfwWindowController.h"
#include "openGL/viewControllerOpenGL.h"
#include "openGL/services/shaderService.h"
#include "openGL/renderer.h"
#include "openGL/windowService.h"
#include "project.h"
#include "projectDataContext.h"

namespace dory
{
    namespace entity = dory::domain::entity;

    //TODO: make inplace factory implementation next to the Controller
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

    //TODO: make inplace factory implementation next to the Controller
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

namespace testApp::registry
{
    namespace domain = dory::domain;
    namespace entity = domain::entity;
    namespace services = domain::services;
    namespace events = domain::events;
    namespace openGL = dory::openGL;
    namespace configuration = dory::configuration;
    namespace win32 = dory::win32;

    using DataContextType = ProjectDataContext;
    using IdType = entity::IdType;
    using ConfigurationServiceType = configuration::FileSystemBasedConfiguration2;
    using CameraRepositoryType = domain::EntityRepository2<entity::Camera, IdType>;
    using ViewRepositoryType = domain::EntityRepository2<entity::View, IdType>;
    using WindowRepositoryType = domain::EntityRepository2<openGL::GlfwWindow, IdType>;
    using PipelineRepositoryType = domain::services::PipelineRepository<entity::PipelineNode, IdType>;
    using EngineType = domain::Engine2<DataContextType, PipelineRepositoryType>;
    using FrameServiceType = services::BasicFrameService2<DataContextType, EngineType>;
    using ControllerInterfaceType = domain::Controller2<DataContextType>;
    using ConsoleControllerType = win32::ConsoleController2<DataContextType>;
    using WindowControllerType = openGL::GlfwWindowController2<DataContextType, WindowRepositoryType>;
    using ConsoleControllerFactoryType = dory::ServiceFactory<ConsoleControllerType, ControllerInterfaceType>;
    using WindowControllerFactoryType = dory::ServiceFactory<WindowControllerType, ControllerInterfaceType>;
    using PipelineManagerType = services::PipelineManager<DataContextType, ConsoleControllerFactoryType, WindowControllerFactoryType, PipelineRepositoryType>;
    using ShaderServiceType = openGL::services::ShaderService2<ConfigurationServiceType>;
    using RendererType = openGL::Renderer2<openGL::RendererDependencies<ShaderServiceType>>;
    using RendererFactoryType = RendererType::FactoryType;
    using ViewControllerType = openGL::ViewControllerOpenGL2<openGL::ViewControllerDependencies<DataContextType,
            RendererType,
            ViewRepositoryType,
            WindowRepositoryType,
            RendererFactoryType>>;
    using ViewControllerFactoryType = ViewControllerType::FactoryType;
    using EngineEventDispatcherType = events::EngineEventHubDispatcher<DataContextType>;
    using EngineEventHubType = events::EngineEventHub<DataContextType>;
    using ConsoleEventDispatcherType = events::SystemConsoleEventHubDispatcher<DataContextType>;
    using ConsoleEventHubType = events::SystemConsoleEventHub<DataContextType>;
    using WindowEventHubDispatcherType = events::WindowEventHubDispatcher<DataContextType>;
    using WindowEventHubType = events::WindowEventHub<DataContextType>;
    using WindowServiceType = openGL::WindowService<openGL::WindowServiceDependencies<WindowRepositoryType >>;
    using ViewServiceType = services::ViewService<services::ViewServiceDependencies<DataContextType,
                                                                                    ViewRepositoryType,
                                                                                    PipelineRepositoryType,
                                                                                    CameraRepositoryType,
                                                                                    ViewControllerFactoryType>>;
    using ProjectType = testApp::Project2<testApp::ProjectDependencies<DataContextType,
            EngineType,
            FrameServiceType,
            EngineEventHubType,
            ConsoleEventHubType,
            WindowEventHubType,
            PipelineManagerType,
            WindowServiceType,
            ViewServiceType>>;

    class Services
    {
    public:
        EngineEventDispatcherType engineEventDispatcher;
        ConsoleEventDispatcherType consoleEventDispatcher;
        WindowEventHubDispatcherType windowEventDispatcher;
        PipelineRepositoryType pipelineRepository;
        CameraRepositoryType cameraRepository;
        ViewRepositoryType viewRepository;
        WindowRepositoryType windowRepository;
        EngineType engine = EngineType{ engineEventDispatcher, pipelineRepository };
        FrameServiceType frameService = FrameServiceType{ engine };
        std::string configurationPath;
        ConfigurationServiceType configurationService = ConfigurationServiceType{ configurationPath };
        ShaderServiceType shaderService = ShaderServiceType{ configurationService };
        ConsoleControllerFactoryType consoleControllerFactory = ConsoleControllerFactoryType{ consoleEventDispatcher };
        WindowControllerFactoryType windowControllerFactory = WindowControllerFactoryType {windowRepository, windowEventDispatcher};
        RendererFactoryType rendererFactory = RendererFactoryType { shaderService };
        ViewControllerFactoryType viewControllerFactory = ViewControllerFactoryType{ rendererFactory, viewRepository, windowRepository };
        PipelineManagerType pipelineManager = PipelineManagerType{ consoleControllerFactory, windowControllerFactory, pipelineRepository };
        WindowServiceType windowService = WindowServiceType{ windowRepository };
        ViewServiceType viewService = ViewServiceType{ viewRepository, pipelineRepository, cameraRepository, viewControllerFactory };
        ProjectType project = ProjectType{ engine, frameService, engineEventDispatcher, consoleEventDispatcher, windowEventDispatcher, pipelineManager, windowService, viewService };

        explicit Services(std::string configurationPath):
                configurationPath(std::move(configurationPath))
        {}
    };

    class ServicesLocal: public Services
    {
    public:
        ServicesLocal():
                Services("configuration")
        {}
    };
}