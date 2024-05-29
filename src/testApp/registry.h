#pragma once

#include "dependencies.h"
#include "projectData.h"

namespace testApp::registry
{
    namespace domain = dory::domain;
    namespace entity = domain::entity;
    namespace services = domain::services;
    namespace events = domain::events;
    namespace devices = domain::devices;
    namespace openGL = dory::openGL;
    namespace configuration = dory::configuration;

    using DataContextType = ProjectDataContext;
    using ConfigurationType = dory::configuration::Configuration;

    struct Events
    {
        using EngineDispatcherType = events::engine::Dispatcher<DataContextType>;
        using EngineType = events::engine::Hub<DataContextType>;
        using WindowDispatcherType = events::window::Dispatcher<DataContextType>;
        using WindowType = events::window::Hub<DataContextType>;
        using ApplicationDispatcherType = events::application::Dispatcher<DataContextType>;
        using ApplicationType = events::application::Hub<DataContextType>;
        using StandartInputDispatcherType = events::io::Dispatcher<DataContextType>;
        using StandartInputType = events::io::Hub<DataContextType>;
        using ScriptDispatcherType = events::script::Dispatcher<DataContextType>;
        using ScriptType = events::script::Hub<DataContextType>;

        EngineDispatcherType engineDispatcher;
        EngineType& engine = engineDispatcher;
        WindowDispatcherType windowDispatcher;
        WindowType& window = windowDispatcher;
        ApplicationDispatcherType applicationDispatcher;
        ApplicationType& application = applicationDispatcher;
        StandartInputDispatcherType standartIoDispatcher;
        StandartInputType& standartInput = standartIoDispatcher;
        ScriptDispatcherType scriptDispatcher;
        ScriptType& script = scriptDispatcher;
    };

    struct Logging
    {
        using LogServiceType = services::MultiSinkLogService;

        LogServiceType appConfigurationLogger;
        LogServiceType appLogger;
    };

    using CameraRepositoryType = domain::EntityRepository<entity::Camera>;
    using ViewRepositoryType = domain::EntityRepository<entity::View>;
    using WindowRepositoryType = domain::EntityRepository<openGL::GlfwWindow>;
    using PipelineRepositoryType = domain::services::PipelineRepository<DataContextType, entity::PipelineNode<DataContextType>>;
    using EngineType = domain::Engine<DataContextType, PipelineRepositoryType>;
    using FrameServiceType = services::BasicFrameService;
    using WindowControllerType = openGL::GlfwWindowController<DataContextType, WindowRepositoryType>;
    using WindowControllerFactoryType = WindowControllerType::FactoryType;
    using PipelineManagerType = services::PipelineManager<DataContextType, WindowControllerFactoryType, PipelineRepositoryType>;
    using ShaderServiceType = openGL::services::ShaderService;
    using RendererType = openGL::Renderer<openGL::RendererDependencies<ShaderServiceType>>;
    using RendererFactoryType = RendererType::FactoryType;
    using ViewControllerType = openGL::ViewControllerOpenGL<openGL::ViewControllerDependencies<DataContextType,
            RendererType,
            ViewRepositoryType,
            WindowRepositoryType,
            RendererFactoryType>>;
    using ViewControllerFactoryType = ViewControllerType::FactoryType;

    using WindowServiceType = openGL::WindowService<openGL::WindowServiceDependencies<WindowRepositoryType >>;
    using ViewServiceType = services::ViewService<services::ViewServiceDependencies<DataContextType,
                                                                                    ViewRepositoryType,
                                                                                    PipelineRepositoryType,
                                                                                    CameraRepositoryType,
                                                                                    ViewControllerFactoryType>>;
#ifdef WIN32
    using StandartIODeviceType = devices::ConsoleIODeviceWin32<DataContextType>;
#endif
#ifdef __unix__
    using StandartIODeviceType = devices::ConsoleIODeviceUnix<DataContextType>;
#endif
    using TerminalDeviceType = devices::TerminalDevice<DataContextType, StandartIODeviceType>;
    using ScriptServiceType = services::ScriptService<DataContextType>;
    using ConfigurationLoaderType = services::YamlConfigurationLoader<ConfigurationType, Logging::LogServiceType>;

    class Services
    {
    public:
        Events events;
        Logging logging;
        ConfigurationLoaderType configurationLoader = ConfigurationLoaderType{logging.appConfigurationLogger};
        PipelineRepositoryType pipelineRepository;
        CameraRepositoryType cameraRepository;
        ViewRepositoryType viewRepository;
        WindowRepositoryType windowRepository;
        ShaderServiceType shaderService;
        WindowControllerFactoryType windowControllerFactory = WindowControllerFactoryType {windowRepository, events.windowDispatcher};
        RendererFactoryType rendererFactory = RendererFactoryType { shaderService };
        ViewControllerFactoryType viewControllerFactory = ViewControllerFactoryType{ rendererFactory, viewRepository, windowRepository };
        PipelineManagerType pipelineManager = PipelineManagerType{ windowControllerFactory, pipelineRepository };
        WindowServiceType windowService = WindowServiceType{ windowRepository };
        ViewServiceType viewService = ViewServiceType{ viewRepository, pipelineRepository, cameraRepository, viewControllerFactory };
        StandartIODeviceType standartIODevice = StandartIODeviceType{events.standartIoDispatcher};
        TerminalDeviceType terminalDevice = TerminalDeviceType{standartIODevice, events.standartInput, events.scriptDispatcher, events.applicationDispatcher};
        ScriptServiceType scriptService = ScriptServiceType{};

        explicit Services(const ConfigurationType& configuration):
                shaderService(configuration.shaderLoader)
        {}
    };
}