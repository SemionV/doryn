#pragma once

#include <utility>

#include "dependencies.h"

#include "base/serviceContainer.h"
#include "base/domain/configuration.h"
#include "base/domain/events/hub.h"
#include "base/domain/entity.h"
#include "base/domain/entityRepository.h"
#include "base/domain/engine.h"
#include "base/domain/services/frameService.h"
#include "base/domain/services/viewService.h"
#include "base/domain/devices/terminalDevice.h"
#include "base/domain/services/scriptService.h"
#include "base/domain/services/configurationService.h"
#include "base/domain/services/logService.h"
#include "openGL/glfwWindow.h"
#include "openGL/glfwWindowController.h"
#include "openGL/viewControllerOpenGL.h"
#include "openGL/services/shaderService.h"
#include "openGL/renderer.h"
#include "openGL/windowService.h"
#include "projectData.h"

#ifdef WIN32
#include "base/domain/devices/standartIoDeviceWin32.h"
#endif

#ifdef __unix__
#include "base/domain/devices/standartIoDeviceUnix.h"
#endif

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

    using ConfigurationServiceType = configuration::FileSystemBasedConfiguration;
    using CameraRepositoryType = domain::EntityRepository<entity::Camera>;
    using ViewRepositoryType = domain::EntityRepository<entity::View>;
    using WindowRepositoryType = domain::EntityRepository<openGL::GlfwWindow>;
    using PipelineRepositoryType = domain::services::PipelineRepository<DataContextType, entity::PipelineNode<DataContextType>>;
    using EngineType = domain::Engine<DataContextType, PipelineRepositoryType>;
    using FrameServiceType = services::BasicFrameService;
    using WindowControllerType = openGL::GlfwWindowController<DataContextType, WindowRepositoryType>;
    using WindowControllerFactoryType = WindowControllerType::FactoryType;
    using PipelineManagerType = services::PipelineManager<DataContextType, WindowControllerFactoryType, PipelineRepositoryType>;
    using ShaderServiceType = openGL::services::ShaderService<ConfigurationServiceType>;
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
    using LogServiceType = services::RotationLogService;

    class Services
    {
    public:
        Events events;
        PipelineRepositoryType pipelineRepository;
        CameraRepositoryType cameraRepository;
        ViewRepositoryType viewRepository;
        WindowRepositoryType windowRepository;
        const std::string configurationPath;
        const std::string logsPath;
        ConfigurationServiceType configurationService = ConfigurationServiceType{ configurationPath };
        ShaderServiceType shaderService = ShaderServiceType{ configurationService };
        WindowControllerFactoryType windowControllerFactory = WindowControllerFactoryType {windowRepository, events.windowDispatcher};
        RendererFactoryType rendererFactory = RendererFactoryType { shaderService };
        ViewControllerFactoryType viewControllerFactory = ViewControllerFactoryType{ rendererFactory, viewRepository, windowRepository };
        PipelineManagerType pipelineManager = PipelineManagerType{ windowControllerFactory, pipelineRepository };
        WindowServiceType windowService = WindowServiceType{ windowRepository };
        ViewServiceType viewService = ViewServiceType{ viewRepository, pipelineRepository, cameraRepository, viewControllerFactory };
        StandartIODeviceType standartIODevice = StandartIODeviceType{events.standartIoDispatcher};
        TerminalDeviceType terminalDevice = TerminalDeviceType{standartIODevice, events.standartInput, events.scriptDispatcher, events.applicationDispatcher};
        ScriptServiceType scriptService = ScriptServiceType{};
        LogServiceType logService = LogServiceType{"test app", logsPath};

        explicit Services(std::string configurationPath, std::string logsPath):
                configurationPath(std::move(configurationPath)),
                logsPath(std::move(logsPath))
        {}
    };

    class ServicesLocal: public Services
    {
    public:
        ServicesLocal():
                Services("configuration", "logs")
        {}
    };
}