#pragma once

#include "dependencies.h"
#include "projectData.h"

#include "base/serviceContainer.h"
#include "base/domain/configuration.h"
#include "base/domain/events/hub.h"
#include "base/domain/entity.h"
#include "base/domain/entityRepository.h"
#include "base/domain/repositories/pipelineRepository.h"
#include "base/domain/engine.h"
#include "base/domain/services/frameService.h"
#include "base/domain/managers/viewManager.h"
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

#ifdef WIN32
#include "base/domain/devices/standartIoDeviceWin32.h"
#endif

#ifdef __unix__
#include "base/domain/devices/standartIoDeviceUnix.h"
#endif

namespace testApp
{
    namespace domain = dory::domain;
    namespace entity = domain::entity;
    namespace services = domain::services;
    namespace managers = domain::managers;
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

    struct Devices
    {
#ifdef WIN32
        using StandartIODeviceType = devices::ConsoleIODeviceWin32<DataContextType>;
#endif
#ifdef __unix__
        using StandartIODeviceType = devices::ConsoleIODeviceUnix<DataContextType>;
#endif
        using TerminalDeviceType = devices::TerminalDevice<DataContextType, StandartIODeviceType>;

        StandartIODeviceType standartIODevice;
        TerminalDeviceType terminalDevice;

        explicit Devices(Events& events):
                standartIODevice(events.standartIoDispatcher),
                terminalDevice(standartIODevice, events.standartInput, events.scriptDispatcher, events.applicationDispatcher)
        {}
    };

    struct Repositories
    {
        using CameraRepositoryType = domain::EntityRepository<entity::Camera>;
        using ViewRepositoryType = domain::EntityRepository<entity::View>;
        using WindowRepositoryType = domain::EntityRepository<openGL::GlfwWindow>;
        using PipelineRepositoryType = domain::repositories::PipelineRepository<DataContextType, entity::PipelineNode<DataContextType>>;

        CameraRepositoryType cameras;
        ViewRepositoryType views;
        WindowRepositoryType windows;
        PipelineRepositoryType pipelines;
    };

    struct Services
    {
        using LogServiceType = services::MultiSinkLogService;
        using FrameServiceType = services::BasicFrameService;
        using ShaderServiceType = openGL::services::ShaderService;
        using RendererType = openGL::Renderer<openGL::RendererDependencies<ShaderServiceType>>;
        using RendererFactoryType = RendererType::FactoryType;
        using EngineType = domain::Engine<DataContextType, Repositories::PipelineRepositoryType>;
        using WindowServiceType = openGL::WindowService<openGL::WindowServiceDependencies<Repositories::WindowRepositoryType >>;
        using ScriptServiceType = services::ScriptService<DataContextType>;
        using ConfigurationLoaderType = services::YamlConfigurationLoader<ConfigurationType, LogServiceType>;
        using WindowControllerType = openGL::GlfwWindowController<DataContextType, Repositories::WindowRepositoryType>;
        using WindowControllerFactoryType = WindowControllerType::FactoryType;
        using ViewControllerType = openGL::ViewControllerOpenGL<openGL::ViewControllerDependencies<DataContextType,
                Services::RendererType,
                Repositories::ViewRepositoryType,
                Repositories::WindowRepositoryType,
                Services::RendererFactoryType>>;
        using ViewControllerFactoryType = ViewControllerType::FactoryType;

        LogServiceType appConfigurationLogger;
        LogServiceType appLogger = LogServiceType{};
        ConfigurationLoaderType configurationLoader;
        ShaderServiceType shaderService;
        RendererFactoryType rendererFactory;
        WindowServiceType windowService;
        ScriptServiceType scriptService;
        WindowControllerFactoryType windowControllerFactory;
        ViewControllerFactoryType viewControllerFactory;

        Services(const ConfigurationType& configuration, Events& events, Repositories& repository):
                configurationLoader(appConfigurationLogger),
                shaderService(configuration.shaderLoader),
                windowService(repository.windows),
                rendererFactory(shaderService),
                windowControllerFactory(repository.windows, events.windowDispatcher),
                viewControllerFactory(rendererFactory, repository.views, repository.windows)
        {}
    };

    struct Managers
    {
        using PipelineManagerType = managers::PipelineManager<DataContextType, Services::WindowControllerFactoryType, Repositories::PipelineRepositoryType>;
        using ViewManagerType = managers::ViewManager<managers::ViewManagerDependencies<DataContextType,
                Repositories::ViewRepositoryType,
                Repositories::PipelineRepositoryType,
                Repositories::CameraRepositoryType,
                Services::ViewControllerFactoryType>>;

        PipelineManagerType pipelineManager;
        ViewManagerType viewManager;

        Managers(Repositories& repository, Services& services):
                pipelineManager(services.windowControllerFactory, repository.pipelines),
                viewManager(repository.views, repository.pipelines, repository.cameras, services.viewControllerFactory)
        {}
    };

    class Registry
    {
    public:
        Events events;
        Devices devices;
        Repositories respository;
        Services services;
        Managers managers;

        explicit Registry(const ConfigurationType& configuration):
                devices(events),
                services(configuration, events, respository),
                managers(respository, services)
        {}
    };
}