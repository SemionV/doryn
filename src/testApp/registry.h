#pragma once

#include "dependencies.h"
#include "projectData.h"

#include <dory/application.h>

#include "base/serviceContainer.h"
#include "base/domain/configuration.h"
#include "base/domain/events/hub.h"
#include "base/domain/entity.h"
#include "base/domain/entityRepository.h"
#include "base/domain/repositories/pipelineRepository.h"
#include "base/domain/engine.h"
#include "base/domain/services/frameService.h"
#include "base/domain/managers/viewManager.h"
#include "base/domain/services/scriptService.h"
#include "base/domain/services/configurationService.h"
#include "base/domain/services/logService.h"
#include "base/domain/services/fileService.h"
#include "base/domain/services/serializationService.h"
#include "base/domain/services/localizationService.h"
#include "base/domain/services/moduleService.h"
#include "base/domain/resources/localization.h"

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

    struct EventLayer: public dory::EventLayer<dory::EventLayerTypeRegistry<DataContextType>>
    {};

    using RepositoryTypes = dory::RepositoryLayerTypeRegistry<DataContextType>;
    struct RepositoryLayer: public dory::RepositoryLayer<RepositoryTypes>
    {};

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
        using EngineType = domain::Engine<DataContextType, RepositoryTypes::PipelineRepositoryType>;
        using WindowServiceType = openGL::WindowService<openGL::WindowServiceDependencies<RepositoryTypes::WindowRepositoryType >>;
        using ScriptServiceType = services::ScriptService<DataContextType>;
        using ConfigurationServiceType = services::configuration::ConfigurationService<LogServiceType, FileServiceType, SerializationServiceBundle, FormatKeyConverterType>;
        using LocalizationServiceType = services::localization::LocalizationService<LogServiceType, FileServiceType, SerializationServiceBundle, FormatKeyConverterType>;
        using WindowControllerType = openGL::GlfwWindowController<DataContextType, RepositoryTypes::WindowRepositoryType>;
        using WindowControllerFactoryType = WindowControllerType::FactoryType;
        using ViewControllerType = openGL::ViewControllerOpenGL<openGL::ViewControllerDependencies<DataContextType,
                ServiceLayer::RendererType,
                RepositoryTypes::ViewRepositoryType,
                RepositoryTypes::WindowRepositoryType,
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
        using PipelineManagerType = managers::PipelineManager<DataContextType, ServiceLayer::WindowControllerFactoryType, RepositoryTypes::PipelineRepositoryType>;
        using ViewManagerType = managers::ViewManager<managers::ViewManagerDependencies<DataContextType,
                RepositoryTypes::ViewRepositoryType,
                RepositoryTypes::PipelineRepositoryType,
                RepositoryTypes::CameraRepositoryType,
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
        EventLayer events = testApp::EventLayer{};
        dory::DeviceLayer<dory::DeviceLayerTypeRegistry<DataContextType>, EventLayer> devices;
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