#pragma once

#include <dory/generic/registryLayer2.h>
#include <dory/containers/hashId.h>
#include "events/eventTypes.h"
#include <dory/core/resources/dataFormat.h>
#include <dory/core/resources/logger.h>
#include <dory/core/resources/entities/camera.h>
#include <dory/core/resources/systemTypes.h>
#include <dory/generic/extension/registryResourceScope.h>

#include <dory/core/devices/iStandardIODevice.h>
#include <dory/core/devices/iTerminalDevice.h>
#include "dory/core/devices/iGpuDevice.h"
#include <dory/core/devices/iFileWatcherDevice.h>
#include <dory/core/devices/iWindowSystemDevice.h>
#include <dory/core/devices/iDisplaySystemDevice.h>
#include <dory/core/devices/iBlockQueueStreamDevice.h>

#include <dory/core/repositories/iViewRepository.h>
#include <dory/core/repositories/iWindowRepository.h>
#include <dory/core/repositories/iShaderRepository.h>
#include <dory/core/repositories/iShaderProgramRepository.h>
#include <dory/core/repositories/assets/iMeshRepository.h>
#include <dory/core/repositories/iSceneRepository.h>
#include <dory/core/repositories/iGraphicalContextRepository.h>
#include <dory/core/repositories/assets/iMeshRepository.h>
#include <dory/core/repositories/assets/iMaterialRepository.h>
#include <dory/core/repositories/assets/iShaderRepository.h>
#include <dory/core/repositories/bindings/iMeshBindingRepository.h>
#include <dory/core/repositories/bindings/iBufferBindingRepository.h>
#include <dory/core/repositories/bindings/iShaderBindingRepository.h>
#include <dory/core/repositories/bindings/iMaterialBindingRepository.h>
#include <dory/core/repositories/iImageStreamRepository.h>
#include <dory/core/repositories/iRepository.h>
#include <dory/core/repositories/iPipelineRepository.h>
#include <dory/core/repositories/iSceneConfigurationRepository.h>
#include <dory/core/repositories/iLayoutRepository.h>
#include <dory/core/repositories/iDisplayRepository.h>

#include <dory/core/services/iSerializer.h>
#include <dory/core/services/iDataFormatResolver.h>
#include <dory/core/services/iScriptService.h>
#include <dory/core/services/iConfigurationService.h>
#include <dory/core/services/iLocalizationService.h>
#include <dory/core/services/iLoopService.h>
#include <dory/core/services/iPipelineService.h>
#include <dory/core/services/iWindowService.h>
#include <dory/core/services/graphics/iRenderer.h>
#include <dory/core/services/iAssetTypeResolver.h>
#include <dory/core/services/iAssetReloadHandler.h>
#include <dory/core/services/graphics/iShaderService.h>
#include <dory/core/services/iSceneService.h>
#include <dory/core/services/iSceneQueryService.h>
#include <dory/core/services/iViewService.h>
#include <dory/core/services/generators/iMeshGenerator.h>
#include <dory/core/services/iImageStreamService.h>
#include <dory/core/services/iLogService.h>
#include <dory/core/services/iAssetBinder.h>
#include <dory/core/services/iFileService.h>
#include <dory/core/services/iLibraryService.h>
#include <dory/core/services/files/iImageFileService.h>
#include <dory/core/services/iProfilingService.h>
#include <dory/core/services/iSceneBuilder.h>
#include <dory/core/services/iSceneConfigurationService.h>
#include <dory/core/services/iObjectFactory.h>
#include <dory/core/services/iSceneDirector.h>
#include <dory/core/services/iLayoutService.h>
#include <dory/core/services/iLayoutSetupService.h>
#include <dory/core/services/iDisplayService.h>

namespace dory::core
{
    namespace repositories
    {
        using ICameraRepository = IRepository<resources::entities::Camera>;
    }

    using containers::hash::operator""_id;

    using WindowSystemList = generic::ValueList<resources::WindowSystem, resources::WindowSystem::glfw, resources::WindowSystem::win32, resources::WindowSystem::x, resources::WindowSystem::wayland>;
    using DisplaySystemList = generic::ValueList<resources::DisplaySystem, resources::DisplaySystem::glfw>;
    using GraphicalSystemList = generic::ValueList<resources::GraphicalSystem, resources::GraphicalSystem::directx, resources::GraphicalSystem::opengl, resources::GraphicalSystem::vulkan>;
    using EcsTypeList = generic::ValueList<resources::EcsType, resources::EcsType::entt, resources::EcsType::dory>;
    using LoggerList = generic::ValueList<resources::Logger, resources::Logger::App, resources::Logger::Config>;
    using DataFormatList = generic::ValueList<resources::DataFormat, resources::DataFormat::json, resources::DataFormat::yaml, resources::DataFormat::unknown>;
    using AssetFileFormatList = generic::ValueList<resources::AssetFileFormat, resources::AssetFileFormat::bmp, resources::AssetFileFormat::png>;
    using AssetReloadHandlerList = generic::ValueList<std::uint64_t, "extension"_id>;
    using AssetBinderList = generic::ValueList<std::uint64_t, "mesh"_id, "material"_id, "shader"_id>;
    using PipelineTriggerList = generic::ValueList<std::uint64_t, "TimeFrameTrigger"_id>;
    using PipelineControllerList = generic::ValueList<std::uint64_t,
        "ViewController"_id,
        "FrameCounter"_id,
        "Profiler"_id,
        "EventDispatcher"_id,
        "StateUpdater"_id,
        "AccelerationMovementController"_id,
        "MovementController"_id,
        "TransformController"_id,
        "ConsoleFlusher"_id,
        "WindowSystemController"_id
    >;
    using DeviceTypeList = generic::ValueList<std::uint64_t,
        "StandardIODevice"_id,
        "TerminalDevice"_id,
        "GlfwWindowSystemDevice"_id,
        "GlfwDisplaySystemDevice"_id,
        "FileWatcherDevice"_id,
        "OpenglGpuDevice"_id,
        "ImageStreamDevice"_id
    >;
    using SceneDirectorTypeList = generic::ValueList<std::uint64_t,
        "AssetLoader"_id,
        "ApplicationDirector"_id,
        "ViewDirector"_id
    >;

    using ServiceListType = generic::TypeList<
        /*Events*/
        generic::registry::ServiceEntry<events::pipeline::Bundle::IDispatcher>,
        generic::registry::ServiceEntry<events::pipeline::Bundle::IListener>,
        generic::registry::ServiceEntry<events::application::Bundle::IDispatcher>,
        generic::registry::ServiceEntry<events::application::Bundle::IListener>,
        generic::registry::ServiceEntry<events::io::Bundle::IDispatcher>,
        generic::registry::ServiceEntry<events::io::Bundle::IListener>,
        generic::registry::ServiceEntry<events::script::Bundle::IDispatcher>,
        generic::registry::ServiceEntry<events::script::Bundle::IListener>,
        generic::registry::ServiceEntry<events::window::Bundle::IDispatcher>,
        generic::registry::ServiceEntry<events::window::Bundle::IListener>,
        generic::registry::ServiceEntry<events::display::Bundle::IDispatcher>,
        generic::registry::ServiceEntry<events::display::Bundle::IListener>,
        generic::registry::ServiceEntry<events::filesystem::Bundle::IDispatcher>,
        generic::registry::ServiceEntry<events::filesystem::Bundle::IListener>,
        generic::registry::ServiceEntry<events::scene::Bundle::IDispatcher>,
        generic::registry::ServiceEntry<events::scene::Bundle::IListener>,
        /*Devices*/
        generic::registry::ServiceEntry<devices::IStandardIODevice>,
        generic::registry::ServiceEntry<devices::ITerminalDevice>,
        generic::registry::ServiceEntry<devices::IWindowSystemDevice, WindowSystemList>,
        generic::registry::ServiceEntry<devices::IDisplaySystemDevice, DisplaySystemList>,
        generic::registry::ServiceEntry<devices::IFileWatcherDevice>,
        generic::registry::ServiceEntry<devices::IGpuDevice, GraphicalSystemList>,
        generic::registry::ServiceEntry<devices::IImageStreamDevice>,
        /*Repositories*/
        generic::registry::ServiceEntry<repositories::ICameraRepository>,
        generic::registry::ServiceEntry<repositories::IViewRepository>,
        generic::registry::ServiceEntry<repositories::IWindowRepository, WindowSystemList>,
        generic::registry::ServiceEntry<repositories::IPipelineRepository>,
        generic::registry::ServiceEntry<repositories::IImageStreamRepository>,
        generic::registry::ServiceEntry<repositories::ISceneRepository, EcsTypeList>,
        generic::registry::ServiceEntry<repositories::assets::IMeshRepository>,
        generic::registry::ServiceEntry<repositories::assets::IMaterialRepository>,
        generic::registry::ServiceEntry<repositories::assets::IShaderRepository>,
        generic::registry::ServiceEntry<repositories::IGraphicalContextRepository>,
        generic::registry::ServiceEntry<repositories::IShaderRepository, GraphicalSystemList>,
        generic::registry::ServiceEntry<repositories::IShaderProgramRepository, GraphicalSystemList>,
        generic::registry::ServiceEntry<repositories::bindings::IMeshBindingRepository, GraphicalSystemList>,
        generic::registry::ServiceEntry<repositories::bindings::IBufferBindingRepository, GraphicalSystemList>,
        generic::registry::ServiceEntry<repositories::bindings::IShaderBindingRepository, GraphicalSystemList>,
        generic::registry::ServiceEntry<repositories::bindings::IMaterialBindingRepository, GraphicalSystemList>,
        generic::registry::ServiceEntry<repositories::ISceneConfigurationRepository>,
        generic::registry::ServiceEntry<repositories::ILayoutRepository>,
        generic::registry::ServiceEntry<repositories::IDisplayRepository, DisplaySystemList>,
        /*Services*/
        generic::registry::ServiceEntry<services::ILibraryService>,
        generic::registry::ServiceEntry<services::IFileService>,
        generic::registry::ServiceEntry<services::IMultiSinkLogService, LoggerList>,
        generic::registry::ServiceEntry<services::ILogService, LoggerList>,
        generic::registry::ServiceEntry<services::serialization::ISerializer, DataFormatList>,
        generic::registry::ServiceEntry<services::IDataFormatResolver>,
        generic::registry::ServiceEntry<services::IScriptService>,
        generic::registry::ServiceEntry<services::IConfigurationService>,
        generic::registry::ServiceEntry<services::ILocalizationService>,
        generic::registry::ServiceEntry<services::IPipelineService>,
        generic::registry::ServiceEntry<services::ILoopService>,
        generic::registry::ServiceEntry<services::graphics::IRenderer>,
        generic::registry::ServiceEntry<services::graphics::IShaderService, GraphicalSystemList>,
        generic::registry::ServiceEntry<services::IViewService>,
        generic::registry::ServiceEntry<services::IWindowService>,
        generic::registry::ServiceEntry<services::IAssetTypeResolver>,
        generic::registry::ServiceEntry<services::IAssetReloadHandler, AssetReloadHandlerList>,
        generic::registry::ServiceEntry<services::ISceneService, EcsTypeList>,
        generic::registry::ServiceEntry<services::ISceneQueryService, EcsTypeList>,
        generic::registry::ServiceEntry<services::graphics::IAssetBinder, AssetBinderList>,
        generic::registry::ServiceEntry<services::generators::IMeshGenerator>,
        generic::registry::ServiceEntry<services::IImageStreamService>,
        generic::registry::ServiceEntry<services::files::IImageFileService, AssetFileFormatList>,
        generic::registry::ServiceEntry<services::IProfilingService>,
        generic::registry::ServiceEntry<services::ISceneBuilder>,
        generic::registry::ServiceEntry<services::ISceneConfigurationService>,
        generic::registry::ServiceEntry<services::ILayoutService>,
        generic::registry::ServiceEntry<services::ILayoutSetupService>,
        generic::registry::ServiceEntry<services::IDisplayService>,
        generic::registry::ServiceEntry<services::IObjectFactory<ITrigger>, PipelineTriggerList>,
        generic::registry::ServiceEntry<services::IObjectFactory<IController>, PipelineControllerList>,
        generic::registry::ServiceEntry<services::IObjectFactory<devices::IDevice>, DeviceTypeList>,
        generic::registry::ServiceEntry<services::IObjectFactory<services::ISceneDirector>, SceneDirectorTypeList>
    >;

    class Registry: public generic::registry::RegistryLayer<ServiceListType>
    {};

    template<typename T>
    using RegistryResourceScope = generic::extension::RegistryResourceScope<T, Registry>;

    template<typename T, typename TIdentifier>
    using RegistryResourceScopeWithId = generic::extension::RegistryResourceScope<T, Registry, TIdentifier>;

    using SerializationContextPoliciesType = generic::serialization::ContextPolicies<Registry, resources::DataContext, resources::DataFormat>;
}