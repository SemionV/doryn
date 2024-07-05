#pragma once

#include "engine.h"

namespace dory
{
    template<typename TDataContext = domain::DataContext>
    struct EventTypeRegistry
    {
        using EngineDispatcherType = domain::events::engine::Dispatcher<TDataContext>;
        using EngineType = domain::events::engine::Hub<TDataContext>;
        using WindowDispatcherType = domain::events::window::Dispatcher<TDataContext>;
        using WindowType = domain::events::window::Hub<TDataContext>;
        using ApplicationDispatcherType = domain::events::application::Dispatcher<TDataContext>;
        using ApplicationType = domain::events::application::Hub<TDataContext>;
        using StandartInputDispatcherType = domain::events::io::Dispatcher<TDataContext>;
        using StandartInputType = domain::events::io::Hub<TDataContext>;
        using ScriptDispatcherType = domain::events::script::Dispatcher<TDataContext>;
        using ScriptType = domain::events::script::Hub<TDataContext>;
    };

    template<typename T>
    struct EventLayer
    {
        T::EngineDispatcherType engineDispatcher;
        T::EngineType& engine = engineDispatcher;
        T::WindowDispatcherType windowDispatcher;
        T::WindowType& window = windowDispatcher;
        T::ApplicationDispatcherType applicationDispatcher;
        T::ApplicationType& application = applicationDispatcher;
        T::StandartInputDispatcherType standardIoDispatcher;
        T::StandartInputType& standardInput = standardIoDispatcher;
        T::ScriptDispatcherType scriptDispatcher;
        T::ScriptType& script = scriptDispatcher;
    };

    template<typename TDataContext = domain::DataContext>
    struct DeviceTypeRegistry
    {

        using StandartIODeviceType = domain::devices::ConsoleIODevice<TDataContext>;
        using TerminalDeviceType = domain::devices::TerminalDevice<TDataContext, StandartIODeviceType>;
    };

    template<typename T>
    struct DeviceLayer
    {
        T::StandartIODeviceType standardIoDevice;
        T::TerminalDeviceType terminalDevice;

        template<typename TEventLayer>
        explicit DeviceLayer(TEventLayer& events):
                standardIoDevice(events.standardIoDispatcher),
                terminalDevice{ standardIoDevice, events.standardInput, events.scriptDispatcher, events.applicationDispatcher }
        {}
    };

    template<typename TDataContext = domain::DataContext>
    struct RepositoryTypeRegistry
    {
        using CameraRepositoryType = domain::EntityRepository<domain::entity::Camera>;
        using ViewRepositoryType = domain::EntityRepository<domain::entity::View>;
        using WindowRepositoryType = domain::EntityRepository<opengl::GlfwWindow>;
        using PipelineRepositoryType = domain::repositories::PipelineRepository<TDataContext, domain::entity::PipelineNode<TDataContext>>;
    };

    template<typename T>
    struct RepositoryLayer
    {
        T::CameraRepositoryType cameras;
        T::ViewRepositoryType views;
        T::WindowRepositoryType windows;
        T::PipelineRepositoryType pipelines;
    };

    template<typename TRepositories, typename TDataContext = domain::DataContext>
    struct ServiceTypeRegistry
    {
        using LogServiceType = domain::services::MultiSinkLogService;
        using FrameServiceType = domain::services::BasicFrameService;
        using FileServiceType = domain::services::FileService;
        using YamlSerializationServiceType = domain::services::serialization::YamlSerializationService;
        using JsonSerializationServiceType = domain::services::serialization::JsonSerializationService<4>;
        using FormatKeyConverterType = domain::services::serialization::FormatKeyConverter;
        using SerializationServiceBundle = domain::services::serialization::SerializationServiceBundle<domain::services::serialization::Format, YamlSerializationServiceType, JsonSerializationServiceType>;
        using ShaderServiceType = opengl::services::ShaderService<LogServiceType, FileServiceType>;
        using RendererType = opengl::Renderer<opengl::RendererDependencies<ShaderServiceType>>;
        using RendererFactoryType = RendererType::FactoryType;
        using EngineType = domain::Engine<TDataContext, typename TRepositories::PipelineRepositoryType>;
        using WindowServiceType = opengl::WindowService<opengl::WindowServiceDependencies<typename TRepositories::WindowRepositoryType >>;
        using ScriptServiceType = domain::services::ScriptService<TDataContext>;
        using ConfigurationServiceType = domain::services::configuration::ConfigurationService<LogServiceType, FileServiceType, SerializationServiceBundle, FormatKeyConverterType>;
        using LocalizationServiceType = domain::services::localization::LocalizationService<LogServiceType, FileServiceType, SerializationServiceBundle, FormatKeyConverterType>;
        using WindowControllerType = opengl::GlfwWindowController<TDataContext, typename TRepositories::WindowRepositoryType>;
        using WindowControllerFactoryType = WindowControllerType::FactoryType;
        using ViewControllerType = opengl::ViewControllerOpenGL<opengl::ViewControllerDependencies<TDataContext,
                RendererType,
                typename TRepositories::ViewRepositoryType,
                typename TRepositories::WindowRepositoryType,
                RendererFactoryType>>;
        using ViewControllerFactoryType = ViewControllerType::FactoryType;
    };

    template<typename T>
    struct ServiceLayer
    {
        T::FileServiceType fileService;
        T::YamlSerializationServiceType yamlSerializationService;
        T::JsonSerializationServiceType jsonSerializationService;
        T::FormatKeyConverterType formatKeyConverter;
        T::SerializationServiceBundle serializationServiceBundle;
        T::LogServiceType appConfigurationLogger;
        T::LogServiceType appLogger;
        T::ConfigurationServiceType configurationService;
        T::LocalizationServiceType localizationService;
        T::ShaderServiceType shaderService;
        T::RendererFactoryType rendererFactory;
        T::WindowServiceType windowService;
        T::ScriptServiceType scriptService;
        T::WindowControllerFactoryType windowControllerFactory;
        T::ViewControllerFactoryType viewControllerFactory;

        template<typename TConfiguration, typename TEventLayer, typename TRepositoryLayer>
        ServiceLayer(TConfiguration& configuration, TEventLayer& events, TRepositoryLayer& repository):
                serializationServiceBundle{
                        {
                                {domain::services::serialization::Format::yaml, std::ref(yamlSerializationService)},
                                {domain::services::serialization::Format::json, std::ref(jsonSerializationService)}
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

    template<typename TRepositories, typename TServices, typename TDataContext = domain::DataContext>
    struct ManagerTypeRegistry
    {
        using PipelineManagerType = domain::managers::PipelineManager<TDataContext, typename TServices::WindowControllerFactoryType, typename TRepositories::PipelineRepositoryType>;
        using ViewManagerType = domain::managers::ViewManager<domain::managers::ViewManagerDependencies<TDataContext,
                typename TRepositories::ViewRepositoryType,
                typename TRepositories::PipelineRepositoryType,
                typename TRepositories::CameraRepositoryType,
                typename TServices::ViewControllerFactoryType>>;
    };

    template<typename T>
    struct ManagerLayer
    {
        T::PipelineManagerType pipelineManager;
        T::ViewManagerType viewManager;

        template<typename TRepositoryLayer, typename TServiceLayer>
        ManagerLayer(TRepositoryLayer& repository, TServiceLayer& services):
                pipelineManager(services.windowControllerFactory, repository.pipelines),
                viewManager(repository.views, repository.pipelines, repository.cameras, services.viewControllerFactory)
        {}
    };
}