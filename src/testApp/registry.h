#pragma once

#include "dependencies.h"

#include "base/serviceContainer.h"
#include "base/domain/configuration.h"
#include "base/domain/events/engineEventHub.h"
#include "base/domain/events/applicationEventHub.h"
#include "base/domain/events/scriptEventHub.h"
#include "base/domain/entity.h"
#include "base/domain/entityRepository.h"
#include "base/domain/engine.h"
#include "base/domain/services/frameService.h"
#include "base/domain/services/viewService.h"
#include "base/domain/devices/terminalDevice.h"
#include "base/domain/logic/cliManager.h"
#include "base/domain/devices/standartIoDevice.h"
#include "base/domain/services/scriptService.h"
#include "openGL/glfwWindow.h"
#include "openGL/glfwWindowController.h"
#include "openGL/viewControllerOpenGL.h"
#include "openGL/services/shaderService.h"
#include "openGL/renderer.h"
#include "openGL/windowService.h"
#include "projectDataContext.h"

#ifdef WIN32
#include "base/win32/consoleController.h"
#endif

#ifdef __unix__
#include "base/unix/consoleController.h"
#endif

namespace testApp::registry
{
    namespace domain = dory::domain;
    namespace entity = domain::entity;
    namespace services = domain::services;
    namespace logic = domain::logic;
    namespace events = domain::events;
    namespace devices = domain::devices;
    namespace openGL = dory::openGL;
    namespace configuration = dory::configuration;

    using DataContextType = ProjectDataContext;
    using ConfigurationServiceType = configuration::FileSystemBasedConfiguration;
    using CameraRepositoryType = domain::EntityRepository<entity::Camera>;
    using ViewRepositoryType = domain::EntityRepository<entity::View>;
    using WindowRepositoryType = domain::EntityRepository<openGL::GlfwWindow>;
    using PipelineRepositoryType = domain::services::PipelineRepository<DataContextType, entity::PipelineNode<DataContextType>>;
    using EngineType = domain::Engine<DataContextType, PipelineRepositoryType>;
    using FrameServiceType = services::BasicFrameService;
#ifdef WIN32
    using ConsoleControllerType = dory::win32::ConsoleController<DataContextType>;
    using ConsoleControllerFactoryType = ConsoleControllerType::FactoryType;
#endif
#ifdef __unix__
    using ConsoleControllerType = dory::nunix::ConsoleController<DataContextType>;
    using ConsoleControllerFactoryType = ConsoleControllerType::FactoryType;
#endif
    using WindowControllerType = openGL::GlfwWindowController<DataContextType, WindowRepositoryType>;
    using WindowControllerFactoryType = WindowControllerType::FactoryType;
    using PipelineManagerType = services::PipelineManager<DataContextType, ConsoleControllerFactoryType, WindowControllerFactoryType, PipelineRepositoryType>;
    using ShaderServiceType = openGL::services::ShaderService<ConfigurationServiceType>;
    using RendererType = openGL::Renderer<openGL::RendererDependencies<ShaderServiceType>>;
    using RendererFactoryType = RendererType::FactoryType;
    using ViewControllerType = openGL::ViewControllerOpenGL<openGL::ViewControllerDependencies<DataContextType,
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
    using ApplicationEventDispatcherType = events::ApplicationEventDispatcher<DataContextType>;
    using ApplicationEventHubType = events::ApplicationEventHub<DataContextType>;
    using StandartInputEventDispatcherType = events::InputEventDispatcher<DataContextType>;
    using StandartInputEventHubType = events::InputEventHub<DataContextType>;
    using ScriptEventDispatcherType = events::ScriptEventDispatcher<DataContextType>;
    using ScriptEventHubType = events::ScriptEventHub<DataContextType>;
    using WindowServiceType = openGL::WindowService<openGL::WindowServiceDependencies<WindowRepositoryType >>;
    using ViewServiceType = services::ViewService<services::ViewServiceDependencies<DataContextType,
                                                                                    ViewRepositoryType,
                                                                                    PipelineRepositoryType,
                                                                                    CameraRepositoryType,
                                                                                    ViewControllerFactoryType>>;
    using StandartIODeviceType = devices::ConsoleIODeviceWin32<DataContextType>;
    using TerminalDeviceType = devices::TerminalDevice<DataContextType, StandartIODeviceType>;
    using ScriptServiceType = services::ScriptService<DataContextType>;

    class Services
    {
    public:
        EngineEventDispatcherType engineEventDispatcher;
        EngineEventHubType& engineEventHub = engineEventDispatcher;
        ConsoleEventDispatcherType consoleEventDispatcher;
        ConsoleEventHubType& consoleEventHub = consoleEventDispatcher;
        WindowEventHubDispatcherType windowEventDispatcher;
        WindowEventHubType& windowEventHub = windowEventDispatcher;
        ApplicationEventDispatcherType applicationEventDispatcher;
        ApplicationEventHubType& applicationEventHub = applicationEventDispatcher;
        StandartInputEventDispatcherType standartIoEventDispatcher;
        StandartInputEventHubType& standartInputEventHub = standartIoEventDispatcher;
        ScriptEventDispatcherType scriptEventDispatcher;
        ScriptEventHubType& scriptEventHub = scriptEventDispatcher;
        PipelineRepositoryType pipelineRepository;
        CameraRepositoryType cameraRepository;
        ViewRepositoryType viewRepository;
        WindowRepositoryType windowRepository;
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
        StandartIODeviceType standartIODevice = StandartIODeviceType{standartIoEventDispatcher};
        TerminalDeviceType terminalDevice = TerminalDeviceType{standartIODevice, standartInputEventHub, scriptEventDispatcher, applicationEventDispatcher};
        ScriptServiceType scriptService = ScriptServiceType{};

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