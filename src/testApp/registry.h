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
#include "base/domain/services/fileService.h"
#include "base/domain/services/serializationService.h"
#include "base/domain/services/localizationService.h"
#include "base/domain/services/moduleService.h"
#include "base/domain/resources/localization.h"
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
    namespace resources = dory::domain::resources;

    using DataContextType = ProjectDataContext;
    using ConfigurationType = dory::configuration::Configuration;
    using LocalizationType = resources::Localization;

    struct EventLayer
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
        StandartInputDispatcherType standardIoDispatcher;
        StandartInputType& standardInput = standardIoDispatcher;
        ScriptDispatcherType scriptDispatcher;
        ScriptType& script = scriptDispatcher;
    };

    struct DeviceLayer
    {
#ifdef WIN32
        using StandartIODeviceType = devices::ConsoleIODeviceWin32<DataContextType>;
#endif
#ifdef __unix__
        using StandartIODeviceType = devices::ConsoleIODeviceUnix<DataContextType>;
#endif
        using TerminalDeviceType = devices::TerminalDevice<DataContextType, StandartIODeviceType>;

        StandartIODeviceType standardIoDevice;
        TerminalDeviceType terminalDevice;

        explicit DeviceLayer(EventLayer& events):
                standardIoDevice(events.standardIoDispatcher),
                terminalDevice{ standardIoDevice, events.standardInput, events.scriptDispatcher, events.applicationDispatcher }
        {}
    };

    struct RepositoryLayer
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

    struct ServiceLayer
    {
        using LogServiceType = services::MultiSinkLogService;
        using FrameServiceType = services::BasicFrameService;
        using FileServiceType = services::FileService;
        using YamlSerializationServiceType = services::serialization::YamlSerializationService;
        using JsonSerializationServiceType = services::serialization::JsonSerializationService<4>;
        using FormatKeyConverterType = services::serialization::FormatKeyConverter;
        using SerializationServiceBundle = services::serialization::SerializationServiceBundle<services::serialization::Format, YamlSerializationServiceType, JsonSerializationServiceType>;
        using ShaderServiceType = openGL::services::ShaderService<LogServiceType, FileServiceType>;
        using RendererType = openGL::Renderer<openGL::RendererDependencies<ShaderServiceType>>;
        using RendererFactoryType = RendererType::FactoryType;
        using EngineType = domain::Engine<DataContextType, RepositoryLayer::PipelineRepositoryType>;
        using WindowServiceType = openGL::WindowService<openGL::WindowServiceDependencies<RepositoryLayer::WindowRepositoryType >>;
        using ScriptServiceType = services::ScriptService<DataContextType>;
        using ConfigurationServiceType = services::configuration::ConfigurationService<LogServiceType, FileServiceType, SerializationServiceBundle, FormatKeyConverterType>;
        using LocalizationServiceType = services::localization::LocalizationService<LogServiceType, FileServiceType, SerializationServiceBundle, FormatKeyConverterType>;
        using WindowControllerType = openGL::GlfwWindowController<DataContextType, RepositoryLayer::WindowRepositoryType>;
        using WindowControllerFactoryType = WindowControllerType::FactoryType;
        using ViewControllerType = openGL::ViewControllerOpenGL<openGL::ViewControllerDependencies<DataContextType,
                ServiceLayer::RendererType,
                RepositoryLayer::ViewRepositoryType,
                RepositoryLayer::WindowRepositoryType,
                ServiceLayer::RendererFactoryType>>;
        using ViewControllerFactoryType = ViewControllerType::FactoryType;

        FileServiceType fileService;
        YamlSerializationServiceType yamlSerializationService;
        JsonSerializationServiceType jsonSerializationService;
        FormatKeyConverterType formatKeyConverter;
        SerializationServiceBundle serializationServiceBundle;
        LogServiceType appConfigurationLogger;
        LogServiceType appLogger = LogServiceType{};
        ConfigurationServiceType configurationService;
        LocalizationServiceType localizationService;
        ShaderServiceType shaderService;
        RendererFactoryType rendererFactory;
        WindowServiceType windowService;
        ScriptServiceType scriptService;
        WindowControllerFactoryType windowControllerFactory;
        ViewControllerFactoryType viewControllerFactory;

        ServiceLayer(const ConfigurationType& configuration, EventLayer& events, RepositoryLayer& repository):
                serializationServiceBundle{
                    {
                        {services::serialization::Format::yaml, std::ref(yamlSerializationService)},
                        {services::serialization::Format::json, std::ref(jsonSerializationService)}
                    }
                },
                configurationService(appConfigurationLogger, fileService, serializationServiceBundle, formatKeyConverter),
                localizationService(appLogger, fileService, serializationServiceBundle, formatKeyConverter),
                shaderService(configuration.shaderLoader, appLogger, fileService),
                windowService(repository.windows),
                rendererFactory(shaderService),
                windowControllerFactory(repository.windows, events.windowDispatcher),
                viewControllerFactory(rendererFactory, repository.views, repository.windows)
        {}
    };

    struct ManagerLayer
    {
        using PipelineManagerType = managers::PipelineManager<DataContextType, ServiceLayer::WindowControllerFactoryType, RepositoryLayer::PipelineRepositoryType>;
        using ViewManagerType = managers::ViewManager<managers::ViewManagerDependencies<DataContextType,
                RepositoryLayer::ViewRepositoryType,
                RepositoryLayer::PipelineRepositoryType,
                RepositoryLayer::CameraRepositoryType,
                ServiceLayer::ViewControllerFactoryType>>;

        PipelineManagerType pipelineManager;
        ViewManagerType viewManager;

        ManagerLayer(RepositoryLayer& repository, ServiceLayer& services):
                pipelineManager(services.windowControllerFactory, repository.pipelines),
                viewManager(repository.views, repository.pipelines, repository.cameras, services.viewControllerFactory)
        {}
    };

    class Registry
    {
    public:
        EventLayer events = EventLayer{};
        DeviceLayer devices;
        RepositoryLayer repositories = RepositoryLayer{};
        ServiceLayer services;
        ManagerLayer managers;

        explicit Registry(const ConfigurationType& configuration):
                devices{ events },
                services { configuration, events, repositories },
                managers { repositories, services }
        {}
    };
}