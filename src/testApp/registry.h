#pragma once

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
#include "base/domain/services/loggerService.h"
#include "openGL/glfwWindow.h"
#include "openGL/glfwWindowController.h"
#include "openGL/viewControllerOpenGL.h"
#include "openGL/services/shaderService.h"
#include "openGL/renderer.h"
#include "openGL/windowService.h"
#include "projectDataContext.h"

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
    using EngineEventDispatcherType = events::engine::Dispatcher<DataContextType>;
    using EngineEventHubType = events::engine::Hub<DataContextType>;
    using WindowEventHubDispatcherType = events::window::Dispatcher<DataContextType>;
    using WindowEventHubType = events::window::Hub<DataContextType>;
    using ApplicationEventDispatcherType = events::application::Dispatcher<DataContextType>;
    using ApplicationEventHubType = events::application::Hub<DataContextType>;
    using StandartInputEventDispatcherType = events::io::Dispatcher<DataContextType>;
    using StandartInputEventHubType = events::io::Hub<DataContextType>;
    using ScriptEventDispatcherType = events::script::Dispatcher<DataContextType>;
    using ScriptEventHubType = events::script::Hub<DataContextType>;
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

    class Services
    {
    public:
        EngineEventDispatcherType engineEventDispatcher;
        EngineEventHubType& engineEventHub = engineEventDispatcher;
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
        WindowControllerFactoryType windowControllerFactory = WindowControllerFactoryType {windowRepository, windowEventDispatcher};
        RendererFactoryType rendererFactory = RendererFactoryType { shaderService };
        ViewControllerFactoryType viewControllerFactory = ViewControllerFactoryType{ rendererFactory, viewRepository, windowRepository };
        PipelineManagerType pipelineManager = PipelineManagerType{ windowControllerFactory, pipelineRepository };
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