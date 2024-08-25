#pragma once

#include <dory/engine/domain/dataContext.h>
#include <dory/engine/domain/events/hub.h>

#ifdef __unix__
#include <dory/engine/domain/devices/standartIoDeviceUnix.h>
#endif
#ifdef WIN32
#include <dory/engine/domain/devices/standartIoDeviceWin32.h>
#endif

#include <dory/engine/domain/devices/terminalDevice.h>

#include <dory/engine/domain/entityRepository.h>
#include <dory/engine/domain/repositories/pipelineRepository.h>

#include <dory/engine/domain/services/configurationService.h>
#include <dory/engine/domain/services/fileService.h>
#include <dory/engine/domain/services/frameService.h>
#include <dory/engine/domain/services/localizationService.h>
#include <dory/engine/domain/services/logService.h>
#include <dory/engine/domain/services/moduleService.h>
#include <dory/engine/domain/services/scriptService.h>
#include <dory/engine/domain/services/serializationService.h>
#include <dory/engine/domain/services/windowService.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <dory/engine/rendering/opengl/windowService.h>
#include <dory/engine/rendering/opengl/viewControllerOpenGL.h>
#include <dory/engine/rendering/opengl/glfwWindowController.h>

#include <dory/engine/domain/managers/viewManager.h>

namespace dory
{
    template<typename... TDataContextSections>
    struct EventTypeRegistry
    {
        using DataContextType = dory::domain::DataContext<TDataContextSections...>;

        using EngineDispatcherType = domain::events::engine::Dispatcher<DataContextType>;
        using EngineType = domain::events::engine::Hub<DataContextType>;
        using WindowDispatcherType = domain::events::window::Dispatcher<DataContextType>;
        using WindowType = domain::events::window::Hub<DataContextType>;
        using ApplicationDispatcherType = domain::events::application::Dispatcher<DataContextType>;
        using ApplicationType = domain::events::application::Hub<DataContextType>;
        using StandartInputDispatcherType = domain::events::io::Dispatcher<DataContextType>;
        using StandartInputType = domain::events::io::Hub<DataContextType>;
        using ScriptDispatcherType = domain::events::script::Dispatcher<DataContextType>;
        using ScriptType = domain::events::script::Hub<DataContextType>;
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

    template<typename... TDataContextSections>
    struct DeviceTypeRegistry
    {
        using DataContextType = dory::domain::DataContext<TDataContextSections...>;

        using StandartIODeviceType = domain::devices::ConsoleIODevice<DataContextType>;
        using TerminalDeviceType = domain::devices::TerminalDevice<DataContextType, StandartIODeviceType>;
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

    template<typename... TDataContextSections>
    struct RepositoryTypeRegistry
    {
        using DataContextType = dory::domain::DataContext<TDataContextSections...>;

        using CameraRepositoryType = domain::EntityRepository<domain::entity::Camera>;
        using ViewRepositoryType = domain::EntityRepository<domain::entity::View>;
        using WindowRepositoryType = domain::EntityRepository<opengl::GlfwWindow>;
        using PipelineRepositoryType = domain::repositories::PipelineRepository<DataContextType, domain::entity::PipelineNode<DataContextType>>;
    };

    template<typename T>
    struct RepositoryLayer
    {
        T::CameraRepositoryType cameras;
        T::ViewRepositoryType views;
        T::WindowRepositoryType windows;
        T::PipelineRepositoryType pipelines;
    };

    template<typename TRepositories, typename... TDataContextSections>
    struct ServiceTypeRegistry
    {
        using DataContextType = dory::domain::DataContext<TDataContextSections...>;

        using LogServiceType = domain::services::MultiSinkLogService;
        using FrameServiceType = domain::services::BasicFrameService;
        using FileServiceType = domain::services::FileService;
        using YamlSerializationServiceType = domain::services::serialization::YamlSerializationService;
        using JsonSerializationServiceType = domain::services::serialization::JsonSerializationService<4>;
        using FormatKeyConverterType = domain::services::serialization::FormatKeyConverter;
        using SerializationServiceBundle = domain::services::serialization::SerializationServiceBundle<domain::services::serialization::Format, YamlSerializationServiceType, JsonSerializationServiceType>;
        using ShaderServiceType = opengl::services::ShaderService<LogServiceType, FileServiceType, TDataContextSections...>;
        using RendererType = opengl::Renderer<DataContextType, opengl::RendererDependencies<ShaderServiceType>>;
        using RendererFactoryType = RendererType::FactoryType;
        using EngineType = domain::Engine<DataContextType, typename TRepositories::PipelineRepositoryType>;
        using WindowServiceType = opengl::WindowService<opengl::WindowServiceDependencies<typename TRepositories::WindowRepositoryType >>;
        using ScriptServiceType = domain::services::ScriptService<DataContextType>;
        using ConfigurationServiceType = domain::services::configuration::ConfigurationService<LogServiceType, FileServiceType, SerializationServiceBundle, FormatKeyConverterType>;
        using LocalizationServiceType = domain::services::localization::LocalizationService<LogServiceType, FileServiceType, SerializationServiceBundle, FormatKeyConverterType>;
        using WindowControllerType = opengl::GlfwWindowController<DataContextType, typename TRepositories::WindowRepositoryType>;
        using WindowControllerFactoryType = WindowControllerType::FactoryType;
        using ViewControllerType = opengl::ViewControllerOpenGL<opengl::ViewControllerDependencies<DataContextType,
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

        template<typename TEventLayer, typename TRepositoryLayer>
        ServiceLayer(TEventLayer& events, TRepositoryLayer& repository):
                serializationServiceBundle{
                        {
                                {domain::services::serialization::Format::yaml, std::ref(yamlSerializationService)},
                                {domain::services::serialization::Format::json, std::ref(jsonSerializationService)}
                        }
                },
                configurationService(appConfigurationLogger, fileService, serializationServiceBundle, formatKeyConverter),
                localizationService(appLogger, fileService, serializationServiceBundle, formatKeyConverter),
                shaderService(appLogger, fileService),
                windowService(repository.windows),
                rendererFactory(shaderService),
                windowControllerFactory(repository.windows, events.windowDispatcher),
                viewControllerFactory(rendererFactory, repository.views, repository.windows)
        {}
    };

    template<typename TRepositories, typename TServices, typename... TDataContextSections>
    struct ManagerTypeRegistry
    {
        using DataContextType = dory::domain::DataContext<TDataContextSections...>;

        using PipelineManagerType = domain::managers::PipelineManager<DataContextType, typename TServices::WindowControllerFactoryType, typename TRepositories::PipelineRepositoryType>;
        using ViewManagerType = domain::managers::ViewManager<domain::managers::ViewManagerDependencies<DataContextType,
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