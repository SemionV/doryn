#pragma once

#include "dataContext.h"
#include "repositories.h"
#include "controllers/pipelineController.h"
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

namespace dory::engine::services
{
    using LogServiceType = MultiSinkLogService;

    using MainControllerType = controllers::MainController<repositories::PipelineRepositoryType>;

    extern template class IFrameService<BasicFrameService>;
    extern template void IFrameService<BasicFrameService>::startLoop(DataContextType& context, controllers::IMainController<MainControllerType>& engine);
    using FrameServiceType = BasicFrameService;

    extern template class IFileService<FileService>;
    using FileServiceType = FileService;

    using YamlSerializationServiceType = serialization::YamlSerializationService;
    using JsonSerializationServiceType = serialization::JsonSerializationService<4>;
    using FormatKeyConverterType = serialization::FormatKeyConverter;
    using SerializationServiceBundle = serialization::SerializationServiceBundle<serialization::Format, YamlSerializationServiceType, JsonSerializationServiceType>;
    using ShaderServiceType = opengl::ShaderService<LogServiceType, FileServiceType, ProjectDataContext>;
    using RendererType = opengl::Renderer<DataContextType, opengl::RendererDependencies<ShaderServiceType>>;
    using RendererFactoryType = RendererType::FactoryType;
    using WindowServiceType = opengl::GlfwWindowService<opengl::GlfwWindowServiceDependencies<repositories::WindowRepositoryType >>;
    using ScriptServiceType = ScriptService<DataContextType>;
    using ConfigurationServiceType = configuration::ConfigurationService<LogServiceType, FileServiceType, SerializationServiceBundle, FormatKeyConverterType>;
    using LocalizationServiceType = localization::LocalizationService<LogServiceType, FileServiceType, SerializationServiceBundle, FormatKeyConverterType>;
    using WindowControllerType = controllers::opengl::GlfwWindowController<DataContextType, repositories::WindowRepositoryType>;
    using WindowControllerFactoryType = WindowControllerType::FactoryType;
    using ViewControllerType = controllers::opengl::ViewControllerOpenGL<controllers::opengl::ViewControllerDependencies<DataContextType,
            RendererType,
            repositories::ViewRepositoryType,
            repositories::WindowRepositoryType,
            RendererFactoryType>>;
    using ViewControllerFactoryType = ViewControllerType::FactoryType;
    using PipelineServiceType = PipelineService<DataContextType, WindowControllerFactoryType, repositories::PipelineRepositoryType>;
    using ViewServiceType = ViewService<ViewManagerDependencies<DataContextType,
            repositories::ViewRepositoryType,
            repositories::PipelineRepositoryType,
            repositories::CameraRepositoryType,
            ViewControllerFactoryType>>;

    struct ServiceLayer
    {
        FileServiceType fileService;
        YamlSerializationServiceType yamlSerializationService;
        JsonSerializationServiceType jsonSerializationService;
        FormatKeyConverterType formatKeyConverter;
        SerializationServiceBundle serializationServiceBundle;
        LogServiceType appConfigurationLogger;
        LogServiceType appLogger;
        ConfigurationServiceType configurationService;
        LocalizationServiceType localizationService;
        ShaderServiceType shaderService;
        RendererFactoryType rendererFactory;
        WindowServiceType windowService;
        ScriptServiceType scriptService;
        WindowControllerFactoryType windowControllerFactory;
        ViewControllerFactoryType viewControllerFactory;
        PipelineServiceType pipelineService;
        ViewServiceType viewService;

        template<typename TEventLayer, typename TRepositoryLayer>
        ServiceLayer(TEventLayer& events, TRepositoryLayer& repository):
                serializationServiceBundle{
                        {
                                {serialization::Format::yaml, std::ref(yamlSerializationService)},
                                {serialization::Format::json, std::ref(jsonSerializationService)}
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