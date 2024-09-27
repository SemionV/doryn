#pragma once

#include <dory/engineObjects/dataContext.h>
#include "dory/engine/events/eventTypes.h"

#ifdef DORY_PLATFORM_LINUX
#include <dory/engine/devices/standartIoDeviceUnix.h>
#endif
#ifdef DORY_PLATFORM_WIN32
#include <dory/engine/devices/standartIoDeviceWin32.h>
#endif

#include <dory/engine/devices/terminalDevice.h>

#include "dory/engine/repositories/entityRepository.h"
#include <dory/engine/repositories/pipelineRepository.h>

#include <dory/engine/services/configurationService.h>
#include <dory/engine/services/fileService.h>
#include <dory/engine/services/frameService.h>
#include <dory/engine/services/localizationService.h>
#include <dory/engine/services/logService.h>
#include <dory/engine/services/moduleService.h>
#include <dory/engine/services/scriptService.h>
#include <dory/engine/services/serializationService.h>
#include <dory/engine/services/windowService.h>
#include <dory/engine/services/viewService.h>

#include <dory/engine/services/opengl/glfwWindowService.h>
#include <dory/engine/controllers/opengl/viewControllerOpenGL.h>
#include <dory/engine/controllers/opengl/glfwWindowController.h>

namespace dory
{
    namespace engineResources = dory::engine::resources;
    namespace engineEvents = dory::engine::events;
    namespace engineDevices = dory::engine::devices;
    namespace engineServices = dory::engine::services;
    namespace engineRepositories = dory::engine::repositories;
    namespace engineControllers = dory::engine::controllers;

    template<typename... TDataContextSections>
    struct EventTypeRegistry
    {
        using DataContextType = engineResources::DataContext<TDataContextSections...>;

        using EngineDispatcherType = engineEvents::mainController::Dispatcher<DataContextType>;
        using MainControllerType = engineEvents::mainController::Hub<DataContextType>;
        using WindowDispatcherType = engineEvents::window::Dispatcher<DataContextType>;
        using WindowType = engineEvents::window::Hub<DataContextType>;
        using ApplicationDispatcherType = engineEvents::application::Dispatcher<DataContextType>;
        using ApplicationType = engineEvents::application::Hub<DataContextType>;
        using StandartInputDispatcherType = engineEvents::io::Dispatcher<DataContextType>;
        using StandartInputType = engineEvents::io::Hub<DataContextType>;
        using ScriptDispatcherType = engineEvents::script::Dispatcher<DataContextType>;
        using ScriptType = engineEvents::script::Hub<DataContextType>;
    };

    template<typename T>
    struct EventLayer
    {
        T::EngineDispatcherType mainControllerDispatcher;
        T::MainControllerType& mainController = mainControllerDispatcher;
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
        using DataContextType = engineResources::DataContext<TDataContextSections...>;

        using StandartIODeviceType = engineDevices::ConsoleIODevice<DataContextType>;
        using TerminalDeviceType = engineDevices::TerminalDevice<DataContextType, StandartIODeviceType>;
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
        using DataContextType = engineResources::DataContext<TDataContextSections...>;

        using CameraRepositoryType = engineRepositories::EntityRepository<engineResources::entity::Camera>;
        using ViewRepositoryType = engineRepositories::EntityRepository<engineResources::entity::View>;
        using WindowRepositoryType = engineRepositories::EntityRepository<engineResources::opengl::GlfwWindow>;
        using PipelineRepositoryType = engineRepositories::PipelineRepository<DataContextType, engineResources::entity::PipelineNode<DataContextType>>;
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
        using DataContextType = engineResources::DataContext<TDataContextSections...>;

        using LogServiceType = engineServices::MultiSinkLogService;
        using FrameServiceType = engineServices::BasicFrameService;
        using FileServiceType = engineServices::FileService;
        using YamlSerializationServiceType = engineServices::serialization::YamlSerializationService;
        using JsonSerializationServiceType = engineServices::serialization::JsonSerializationService<4>;
        using FormatKeyConverterType = engineServices::serialization::FormatKeyConverter;
        using SerializationServiceBundle = engineServices::serialization::SerializationServiceBundle<engineServices::serialization::Format, YamlSerializationServiceType, JsonSerializationServiceType>;
        using ShaderServiceType = engineServices::opengl::ShaderService<LogServiceType, FileServiceType, TDataContextSections...>;
        using RendererType = engineServices::opengl::Renderer<DataContextType, engineServices::opengl::RendererDependencies<ShaderServiceType>>;
        using RendererFactoryType = RendererType::FactoryType;
        using MainControllerType = engineControllers::MainController<typename TRepositories::PipelineRepositoryType>;
        using WindowServiceType = engineServices::opengl::GlfwWindowService<engineServices::opengl::GlfwWindowServiceDependencies<typename TRepositories::WindowRepositoryType >>;
        using ScriptServiceType = engineServices::ScriptService<DataContextType>;
        using ConfigurationServiceType = engineServices::configuration::ConfigurationService<LogServiceType, FileServiceType, SerializationServiceBundle, FormatKeyConverterType>;
        using LocalizationServiceType = engineServices::localization::LocalizationService<LogServiceType, FileServiceType, SerializationServiceBundle, FormatKeyConverterType>;
        using WindowControllerType = engineControllers::opengl::GlfwWindowController<DataContextType, typename TRepositories::WindowRepositoryType>;
        using WindowControllerFactoryType = WindowControllerType::FactoryType;
        using ViewControllerType = engineControllers::opengl::ViewControllerOpenGL<engineControllers::opengl::ViewControllerDependencies<DataContextType,
                RendererType,
                typename TRepositories::ViewRepositoryType,
                typename TRepositories::WindowRepositoryType,
                RendererFactoryType>>;
        using ViewControllerFactoryType = ViewControllerType::FactoryType;
        using PipelineServiceType = engineServices::PipelineService<DataContextType, WindowControllerFactoryType, typename TRepositories::PipelineRepositoryType>;
        using ViewServiceType = engineServices::ViewService<engineServices::ViewManagerDependencies<DataContextType,
                typename TRepositories::ViewRepositoryType,
                typename TRepositories::PipelineRepositoryType,
                typename TRepositories::CameraRepositoryType,
                ViewControllerFactoryType>>;
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
        T::PipelineServiceType pipelineService;
        T::ViewServiceType viewService;

        template<typename TEventLayer, typename TRepositoryLayer>
        ServiceLayer(TEventLayer& events, TRepositoryLayer& repository):
                serializationServiceBundle{
                        {
                                {engineServices::serialization::Format::yaml, std::ref(yamlSerializationService)},
                                {engineServices::serialization::Format::json, std::ref(jsonSerializationService)}
                        }
                },
                configurationService(appConfigurationLogger, fileService, serializationServiceBundle, formatKeyConverter),
                localizationService(appLogger, fileService, serializationServiceBundle, formatKeyConverter),
                shaderService(appLogger, fileService),
                windowService(repository.windows),
                rendererFactory(shaderService),
                windowControllerFactory(repository.windows, events.windowDispatcher),
                viewControllerFactory(rendererFactory, repository.views, repository.windows),
                pipelineService(windowControllerFactory, repository.pipelines),
                viewService(repository.views, repository.pipelines, repository.cameras, viewControllerFactory)
        {}
    };
}