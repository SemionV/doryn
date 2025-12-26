#ifndef DORY_OPENGL_INCLUDED
#include <glad/gl.h>
#define DORY_OPENGL_INCLUDED
#endif
#include <GLFW/glfw3.h>

#include "dory/game/setup.h"
#include <dory/core/resources/systemTypes.h>
#include <dory/core/resources/localizationRefl.h>
#include <dory/core/resources/scene/componentsRefl.h>
#include <dory/core/resources/scene/configurationRefl.h>
#include <dory/core/resources/objectFactoryRefl.h>

#ifdef DORY_PLATFORM_LINUX
#include <dory/core/devices/standardIoDeviceUnix.h>
#endif
#ifdef DORY_PLATFORM_WIN32
#include <dory/core/devices/standardIoDeviceWin32.h>
#endif
#include <dory/core/controllers/viewController.h>
#include <dory/core/devices/terminalDevice.h>
#include <dory/core/devices/glfwWindowSystemDevice.h>
#include <dory/core/devices/glfwDisplaySystemDevice.h>
#include <dory/core/devices/openglGpuDevice.h>
#include <dory/core/devices/fileWatcherDevice.h>
#include <dory/core/devices/imageStreamDevice.h>

#include <dory/core/repositories/pipelineRepository.h>
#include <dory/core/repositories/imageStreamRepository.h>
#include <dory/core/repositories/viewRepository.h>
#include <dory/core/repositories/windowRepository.h>
#include <dory/core/repositories/shaderRepository.h>
#include <dory/core/repositories/shaderProgramRepository.h>
#include <dory/core/repositories/assets/meshRepository.h>
#include <dory/core/repositories/assets/materialRepository.h>
#include <dory/core/repositories/assets/shaderRepository.h>
#include <dory/core/repositories/graphicalContextRepository.h>
#include <dory/core/repositories/bindings/meshBindingRepository.h>
#include <dory/core/repositories/bindings/bufferBindingRepository.h>
#include <dory/core/repositories/bindings/shaderBindingRepository.h>
#include <dory/core/repositories/bindings/materialBindingRepository.h>
#include <dory/core/repositories/enttSceneRepository.h>
#include <dory/core/repositories/sceneConfigurationRepository.h>
#include <dory/core/repositories/layoutRepository.h>
#include <dory/core/repositories/displayRepository.h>

#include <dory/core/services/fileService.h>
#include <dory/core/services/libraryService.h>
#include <dory/core/services/logServiceNull.h>
#include <dory/core/services/logService.h>
#include <dory/core/services/serializer.h>
#include <dory/core/services/dataFormatResolver.h>
#include <dory/core/services/scriptService.h>
#include <dory/core/services/configurationService.h>
#include <dory/core/services/localizationService.h>
#include <dory/core/services/pipelineService.h>
#include <dory/core/services/loopService.h>
#include <dory/core/services/windowService.h>
#include <dory/core/services/assetTypeResolver.h>
#include <dory/core/services/hot-reload/extensionLoader.h>
#include <dory/core/services/enttSceneService.h>
#include <dory/core/services/enttSceneQueryService.h>
#include <dory/core/services/graphics/meshAssetBinder.h>
#include <dory/core/services/graphics/materialAssetBinder.h>
#include <dory/core/services/graphics/shaderAssetBinder.h>
#include <dory/core/services/viewService.h>
#include <dory/core/services/graphics/renderer.h>
#include <dory/core/services/generators/meshGenerator.h>
#include <dory/core/services/imageStreamService.h>
#include <dory/core/services/files/imageFileService.h>
#include <dory/core/services/profilingService.h>
#include <dory/core/services/sceneConfigurationService.h>
#include <dory/core/services/sceneBuilder.h>
#include <dory/core/services/scene/directors/assetLoader.h>
#include <dory/core/services/scene/directors/applicationDirector.h>
#include <dory/core/services/scene/directors/viewDirector.h>
#include <dory/core/services/layoutSetupService.h>
#include <dory/core/services/layoutService.h>

namespace dory::game
{
    using containers::hash::operator""_id;

    void Setup::setupRegistry(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry,
                                const core::resources::configuration::Configuration& configuration)
    {
        registerEventBundle<core::events::pipeline::Bundle>(libraryHandle, registry);
        registerEventBundle<core::events::application::Bundle>(libraryHandle, registry);
        registerEventBufferBundle<core::events::io::Bundle>(libraryHandle, registry);
        registerEventBundle<core::events::script::Bundle>(libraryHandle, registry);
        registerEventBufferBundle<core::events::window::Bundle>(libraryHandle, registry);
        registerEventBufferBundle<core::events::display::Bundle>(libraryHandle, registry);
        registerEventBufferBundle<core::events::filesystem::Bundle>(libraryHandle, registry);
        registerEventBufferBundle<core::events::scene::Bundle>(libraryHandle, registry);

        registerService<core::devices::IStandardIODevice, core::devices::StandardIODevice>(libraryHandle, registry, registry);
        registerSingletonObjectFactory<core::devices::IDevice, core::devices::IStandardIODevice>("StandardIODevice"_id, libraryHandle, registry);

        registerService<core::devices::ITerminalDevice, core::devices::TerminalDevice>(libraryHandle, registry, registry);
        registerSingletonObjectFactory<core::devices::IDevice, core::devices::ITerminalDevice>("TerminalDevice"_id, libraryHandle, registry);

        registerService<core::devices::IWindowSystemDevice, core::resources::WindowSystem::glfw, core::devices::GlfwWindowSystemDevice>(libraryHandle, registry, registry);
        registerSingletonObjectFactory<core::devices::IDevice, core::devices::IWindowSystemDevice, core::resources::WindowSystem::glfw>("GlfwWindowSystemDevice"_id, libraryHandle, registry);

        registerService<core::devices::IDisplaySystemDevice, core::resources::DisplaySystem::glfw, core::devices::GlfwDisplaySystemDevice>(libraryHandle, registry, registry);
        registerSingletonObjectFactory<core::devices::IDevice, core::devices::IDisplaySystemDevice, core::resources::DisplaySystem::glfw>("GlfwDisplaySystemDevice"_id, libraryHandle, registry);

        registerService<core::devices::IFileWatcherDevice, core::devices::FileWatcherDevice>(libraryHandle, registry, registry);
        registerSingletonObjectFactory<core::devices::IDevice, core::devices::IFileWatcherDevice>("FileWatcherDevice"_id, libraryHandle, registry);

        registerService<core::devices::IGpuDevice, core::resources::GraphicalSystem::opengl, core::devices::OpenglGpuDevice>(libraryHandle, registry, registry);
        registerSingletonObjectFactory<core::devices::IDevice, core::devices::IGpuDevice, core::resources::GraphicalSystem::opengl>("OpenglGpuDevice"_id, libraryHandle, registry);

        registerService<core::devices::IImageStreamDevice, core::devices::ImageStreamDevice>(libraryHandle, registry, registry);
        registerSingletonObjectFactory<core::devices::IDevice, core::devices::IImageStreamDevice>("ImageStreamDevice"_id, libraryHandle, registry);

        registerService<core::repositories::IImageStreamRepository, core::repositories::ImageStreamRepository>(libraryHandle, registry);
        registerService<core::repositories::IViewRepository, core::repositories::ViewRepository>(libraryHandle, registry);
        registerService<core::repositories::IPipelineRepository, core::repositories::PipelineRepository>(libraryHandle, registry);
        registerRepository<core::resources::entities::Camera>(libraryHandle, registry);
        const auto sceneRepository = createInstance<core::repositories::EnttSceneRepository>();
        registry.set<core::repositories::ISceneRepository, core::resources::EcsType::entt>(libraryHandle, sceneRepository);

        const auto windowRepository = createInstance<core::repositories::WindowRepository>();
        registry.set<core::repositories::IWindowRepository, core::resources::WindowSystem::glfw>(libraryHandle, windowRepository);

        registerService<core::repositories::IGraphicalContextRepository, core::repositories::GraphicalContextRepository>(libraryHandle, registry);
        registerService<core::repositories::assets::IMeshRepository, core::repositories::assets::MeshRepository>(libraryHandle, registry);
        registerService<core::repositories::assets::IMaterialRepository, core::repositories::assets::MaterialRepository>(libraryHandle, registry);
        registerService<core::repositories::assets::IShaderRepository, core::repositories::assets::ShaderRepository>(libraryHandle, registry);
        registerService<core::repositories::bindings::IMeshBindingRepository, core::resources::GraphicalSystem::opengl, core::repositories::bindings::MeshBindingRepository>(libraryHandle, registry);
        registerService<core::repositories::bindings::IBufferBindingRepository, core::resources::GraphicalSystem::opengl, core::repositories::bindings::BufferBindingRepository>(libraryHandle, registry);
        registerService<core::repositories::bindings::IShaderBindingRepository, core::resources::GraphicalSystem::opengl, core::repositories::bindings::ShaderBindingRepository>(libraryHandle, registry);
        registerService<core::repositories::bindings::IMaterialBindingRepository, core::resources::GraphicalSystem::opengl, core::repositories::bindings::MaterialBindingRepository>(libraryHandle, registry);
        registerService<core::repositories::ISceneConfigurationRepository, core::repositories::SceneConfigurationRepository>(libraryHandle, registry);
        registerService<core::repositories::ILayoutRepository, core::repositories::LayoutRepository>(libraryHandle, registry);
        registerService<core::repositories::IDisplayRepository, core::resources::DisplaySystem::glfw, core::repositories::DisplayRepository>(libraryHandle, registry);

        registerService<core::repositories::IShaderRepository, core::resources::GraphicalSystem::opengl, core::repositories::ShaderRepository>(libraryHandle, registry);
        registerService<core::repositories::IShaderProgramRepository, core::resources::GraphicalSystem::opengl, core::repositories::ShaderProgramRepository>(libraryHandle, registry);

        registerService<core::services::IFileService, core::services::FileService>(libraryHandle, registry);
        registerService<core::services::ILibraryService, core::services::LibraryService>(libraryHandle, registry, registry);

        const auto appLogger = createInstance<core::services::LogService>();
        registry.set<core::services::IMultiSinkLogService, core::resources::Logger::App>(libraryHandle, appLogger);
        registry.set<core::services::ILogService, core::resources::Logger::App>(libraryHandle, appLogger);

        const auto configLogger = createInstance<core::services::LogService>();
        registry.set<core::services::IMultiSinkLogService, core::resources::Logger::Config>(libraryHandle, configLogger);
        registry.set<core::services::ILogService, core::resources::Logger::Config>(libraryHandle, configLogger);

        registerService<core::services::serialization::ISerializer, core::resources::DataFormat::yaml, core::services::serialization::YamlSerializer>(libraryHandle, registry);
        registerService<core::services::serialization::ISerializer, core::resources::DataFormat::json, core::services::serialization::JsonSerializer>(libraryHandle, registry);
        registerService<core::services::IDataFormatResolver, core::services::DataFormatResolver>(libraryHandle, registry);

        registerService<core::services::IScriptService, core::services::ScriptService>(libraryHandle, registry, registry);
        registerService<core::services::IConfigurationService, core::services::ConfigurationService>(libraryHandle, registry, registry);
        registerService<core::services::ILocalizationService, core::services::LocalizationService>(libraryHandle, registry, registry);
        registerService<core::services::IPipelineService, core::services::PipelineService>(libraryHandle, registry, registry);
        registerService<core::services::ILoopService, core::services::LoopService>(libraryHandle, registry, registry);

        registerService<core::services::IWindowService, core::services::WindowService>(libraryHandle, registry, registry);
        registerService<core::services::IViewService, core::services::ViewService>(libraryHandle, registry, registry);

        registerService<core::services::graphics::IRenderer, core::services::graphics::Renderer>(libraryHandle, registry, registry);
        registerService<core::services::graphics::IAssetBinder, core::services::graphics::MeshAssetBinder>(core::resources::AssetTypeName::mesh, libraryHandle, registry, registry);
        registerService<core::services::graphics::IAssetBinder, core::services::graphics::MaterialAssetBinder>(core::resources::AssetTypeName::material, libraryHandle, registry, registry);
        registerService<core::services::graphics::IAssetBinder, core::services::graphics::ShaderAssetBinder>(core::resources::AssetTypeName::shader, libraryHandle, registry, registry);

        registerService<core::services::IAssetTypeResolver, core::services::AssetTypeResolver>(libraryHandle, registry);
        registerService<core::services::IAssetReloadHandler, core::services::loaders::ExtensionLoader>(core::resources::AssetTypeName::extension, libraryHandle, registry, registry);
        registerService<core::services::generators::IMeshGenerator, core::services::generators::MeshGenerator>(libraryHandle, registry);

        const auto sceneService = createInstance<core::services::EnttSceneService>();
        registry.set<core::services::ISceneService>(libraryHandle, sceneService);
        registry.set<core::services::ISceneService, core::resources::EcsType::entt>(libraryHandle, sceneService);

        const auto sceneQueryService = createInstance<core::services::EnttSceneQueryService>(registry);
        registry.set<core::services::ISceneQueryService, core::resources::EcsType::entt>(libraryHandle, sceneQueryService);

        registerService<core::services::IImageStreamService, core::services::ImageStreamService>(libraryHandle, registry, registry);
        registerService<core::services::files::IImageFileService, core::resources::AssetFileFormat::bmp, core::services::files::BmpImageFileService>(libraryHandle, registry);
        registerService<core::services::files::IImageFileService, core::resources::AssetFileFormat::png, core::services::files::PngImageFileService>(libraryHandle, registry);

        registerService<core::services::IProfilingService, core::services::ProfilingService>(libraryHandle, registry, registry);

        registerService<core::services::ISceneConfigurationService, core::services::SceneConfigurationService>(libraryHandle, registry, registry);
        registerService<core::services::ISceneBuilder, core::services::SceneBuilder>(libraryHandle, registry, registry);

        registerService<core::services::ILayoutSetupService, core::services::LayoutSetupService>(libraryHandle, registry);
        registerService<core::services::ILayoutService, core::services::LayoutService>(libraryHandle, registry);

        registerObjectFactory<core::ITrigger, core::triggers::TimeFrameTrigger>("TimeFrameTrigger"_id, libraryHandle, registry);

        registerObjectFactory<core::IController, core::controllers::ViewController>("ViewController"_id, libraryHandle, registry);
        registerObjectFactory<core::IController, core::controllers::FrameCounter>("FrameCounter"_id, libraryHandle, registry);
        registerObjectFactory<core::IController, core::controllers::Profiler>("Profiler"_id, libraryHandle, registry);
        registerObjectFactory<core::IController, core::controllers::EventDispatcher>("EventDispatcher"_id, libraryHandle, registry);
        registerObjectFactory<core::IController, core::controllers::StateUpdater>("StateUpdater"_id, libraryHandle, registry);
        registerObjectFactory<core::IController, core::controllers::AccelerationMovementController>("AccelerationMovementController"_id, libraryHandle, registry);
        registerObjectFactory<core::IController, core::controllers::MovementController>("MovementController"_id, libraryHandle, registry);
        registerObjectFactory<core::IController, core::controllers::TransformController>("TransformController"_id, libraryHandle, registry);
        registerObjectFactory<core::IController, core::controllers::ConsoleFlusher>("ConsoleFlusher"_id, libraryHandle, registry);
        registerObjectFactory<core::IController, core::controllers::WindowSystemController>("WindowSystemController"_id, libraryHandle, registry);

        registerObjectFactory<core::services::ISceneDirector, core::services::scene::directors::AssetLoader>("AssetLoader"_id, libraryHandle, registry);
        registerObjectFactory<core::services::ISceneDirector, core::services::scene::directors::ApplicationDirector>("ApplicationDirector"_id, libraryHandle, registry);
        registerObjectFactory<core::services::ISceneDirector, core::services::scene::directors::ViewDirector>("ViewDirector"_id, libraryHandle, registry);
    }
}