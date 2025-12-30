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
    using data_structures::hash::operator""_id;

    void Setup::setupRegistry(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry,
                                const core::resources::configuration::Configuration& configuration)
    {
        using namespace core;

        registerEventBundle<events::pipeline::Bundle>(libraryHandle, registry);
        registerEventBundle<events::application::Bundle>(libraryHandle, registry);
        registerEventBufferBundle<events::io::Bundle>(libraryHandle, registry);
        registerEventBundle<events::script::Bundle>(libraryHandle, registry);
        registerEventBufferBundle<events::window::Bundle>(libraryHandle, registry);
        registerEventBufferBundle<events::display::Bundle>(libraryHandle, registry);
        registerEventBufferBundle<events::filesystem::Bundle>(libraryHandle, registry);
        registerEventBufferBundle<events::scene::Bundle>(libraryHandle, registry);

        registerService<devices::IStandardIODevice, devices::StandardIODevice>(libraryHandle, registry, registry);
        registerSingletonObjectFactory<devices::IDevice, devices::IStandardIODevice>("StandardIODevice"_id, libraryHandle, registry);

        registerService<devices::ITerminalDevice, devices::TerminalDevice>(libraryHandle, registry, registry);
        registerSingletonObjectFactory<devices::IDevice, devices::ITerminalDevice>("TerminalDevice"_id, libraryHandle, registry);

        registerService<devices::IWindowSystemDevice, resources::WindowSystem::glfw, devices::GlfwWindowSystemDevice>(libraryHandle, registry, registry);
        registerSingletonObjectFactory<devices::IDevice, devices::IWindowSystemDevice, resources::WindowSystem::glfw>("GlfwWindowSystemDevice"_id, libraryHandle, registry);

        registerService<devices::IDisplaySystemDevice, resources::DisplaySystem::glfw, devices::GlfwDisplaySystemDevice>(libraryHandle, registry, registry);
        registerSingletonObjectFactory<devices::IDevice, devices::IDisplaySystemDevice, resources::DisplaySystem::glfw>("GlfwDisplaySystemDevice"_id, libraryHandle, registry);

        registerService<devices::IFileWatcherDevice, devices::FileWatcherDevice>(libraryHandle, registry, registry, _globalAllocator);
        registerSingletonObjectFactory<devices::IDevice, devices::IFileWatcherDevice>("FileWatcherDevice"_id, libraryHandle, registry);

        registerService<devices::IGpuDevice, resources::GraphicalSystem::opengl, devices::OpenglGpuDevice>(libraryHandle, registry, registry);
        registerSingletonObjectFactory<devices::IDevice, devices::IGpuDevice, resources::GraphicalSystem::opengl>("OpenglGpuDevice"_id, libraryHandle, registry);

        registerService<devices::IImageStreamDevice, devices::ImageStreamDevice>(libraryHandle, registry, registry);
        registerSingletonObjectFactory<devices::IDevice, devices::IImageStreamDevice>("ImageStreamDevice"_id, libraryHandle, registry);

        registerService<repositories::IImageStreamRepository, repositories::ImageStreamRepository>(libraryHandle, registry);
        registerService<repositories::IViewRepository, repositories::ViewRepository>(libraryHandle, registry);
        registerService<repositories::IPipelineRepository, repositories::PipelineRepository>(libraryHandle, registry);
        registerRepository<resources::entities::Camera>(libraryHandle, registry);
        const auto sceneRepository = createInstance<repositories::EnttSceneRepository>();
        registry.set<repositories::ISceneRepository, resources::EcsType::entt>(libraryHandle, sceneRepository);

        const auto windowRepository = createInstance<repositories::WindowRepository>();
        registry.set<repositories::IWindowRepository, resources::WindowSystem::glfw>(libraryHandle, windowRepository);

        registerService<repositories::IGraphicalContextRepository, repositories::GraphicalContextRepository>(libraryHandle, registry);
        registerService<repositories::assets::IMeshRepository, repositories::assets::MeshRepository>(libraryHandle, registry);
        registerService<repositories::assets::IMaterialRepository, repositories::assets::MaterialRepository>(libraryHandle, registry);
        registerService<repositories::assets::IShaderRepository, repositories::assets::ShaderRepository>(libraryHandle, registry);
        registerService<repositories::bindings::IMeshBindingRepository, resources::GraphicalSystem::opengl, repositories::bindings::MeshBindingRepository>(libraryHandle, registry);
        registerService<repositories::bindings::IBufferBindingRepository, resources::GraphicalSystem::opengl, repositories::bindings::BufferBindingRepository>(libraryHandle, registry);
        registerService<repositories::bindings::IShaderBindingRepository, resources::GraphicalSystem::opengl, repositories::bindings::ShaderBindingRepository>(libraryHandle, registry);
        registerService<repositories::bindings::IMaterialBindingRepository, resources::GraphicalSystem::opengl, repositories::bindings::MaterialBindingRepository>(libraryHandle, registry);
        registerService<repositories::ISceneConfigurationRepository, repositories::SceneConfigurationRepository>(libraryHandle, registry);
        registerService<repositories::ILayoutRepository, repositories::LayoutRepository>(libraryHandle, registry);
        registerService<repositories::IDisplayRepository, resources::DisplaySystem::glfw, repositories::DisplayRepository>(libraryHandle, registry);

        registerService<repositories::IShaderRepository, resources::GraphicalSystem::opengl, repositories::ShaderRepository>(libraryHandle, registry);
        registerService<repositories::IShaderProgramRepository, resources::GraphicalSystem::opengl, repositories::ShaderProgramRepository>(libraryHandle, registry);

        registerService<services::IFileService, services::FileService>(libraryHandle, registry);
        registerService<services::ILibraryService, services::LibraryService>(libraryHandle, registry, registry);

        const auto appLogger = createInstance<services::LogService>();
        registry.set<services::IMultiSinkLogService, resources::Logger::App>(libraryHandle, appLogger);
        registry.set<services::ILogService, resources::Logger::App>(libraryHandle, appLogger);

        const auto configLogger = createInstance<services::LogService>();
        registry.set<services::IMultiSinkLogService, resources::Logger::Config>(libraryHandle, configLogger);
        registry.set<services::ILogService, resources::Logger::Config>(libraryHandle, configLogger);

        registerService<services::serialization::ISerializer, resources::DataFormat::yaml, services::serialization::YamlSerializer>(libraryHandle, registry);
        registerService<services::serialization::ISerializer, resources::DataFormat::json, services::serialization::JsonSerializer>(libraryHandle, registry);
        registerService<services::IDataFormatResolver, services::DataFormatResolver>(libraryHandle, registry);

        registerService<services::IScriptService, services::ScriptService>(libraryHandle, registry, registry);
        registerService<services::IConfigurationService, services::ConfigurationService>(libraryHandle, registry, registry);
        registerService<services::ILocalizationService, services::LocalizationService>(libraryHandle, registry, registry);
        registerService<services::IPipelineService, services::PipelineService>(libraryHandle, registry, registry);
        registerService<services::ILoopService, services::LoopService>(libraryHandle, registry, registry);

        registerService<services::IWindowService, services::WindowService>(libraryHandle, registry, registry);
        registerService<services::IViewService, services::ViewService>(libraryHandle, registry, registry);

        registerService<services::graphics::IRenderer, services::graphics::Renderer>(libraryHandle, registry, registry);
        registerService<services::graphics::IAssetBinder, services::graphics::MeshAssetBinder>(resources::AssetTypeName::mesh, libraryHandle, registry, registry);
        registerService<services::graphics::IAssetBinder, services::graphics::MaterialAssetBinder>(resources::AssetTypeName::material, libraryHandle, registry, registry);
        registerService<services::graphics::IAssetBinder, services::graphics::ShaderAssetBinder>(resources::AssetTypeName::shader, libraryHandle, registry, registry);

        registerService<services::IAssetTypeResolver, services::AssetTypeResolver>(libraryHandle, registry);
        registerService<services::IAssetReloadHandler, services::loaders::ExtensionLoader>(resources::AssetTypeName::extension, libraryHandle, registry, registry);
        registerService<services::generators::IMeshGenerator, services::generators::MeshGenerator>(libraryHandle, registry);

        registerService<services::ISceneService, resources::EcsType::entt, services::EnttSceneService>(libraryHandle, registry);

        const auto sceneQueryService = createInstance<services::EnttSceneQueryService>(registry);
        registry.set<services::ISceneQueryService, resources::EcsType::entt>(libraryHandle, sceneQueryService);

        registerService<services::IImageStreamService, services::ImageStreamService>(libraryHandle, registry, registry);
        registerService<services::files::IImageFileService, resources::AssetFileFormat::bmp, services::files::BmpImageFileService>(libraryHandle, registry);
        registerService<services::files::IImageFileService, resources::AssetFileFormat::png, services::files::PngImageFileService>(libraryHandle, registry);

        registerService<services::IProfilingService, services::ProfilingService>(libraryHandle, registry, registry);

        registerService<services::ISceneConfigurationService, services::SceneConfigurationService>(libraryHandle, registry, registry);
        registerService<services::ISceneBuilder, services::SceneBuilder>(libraryHandle, registry, registry);

        registerService<services::ILayoutSetupService, services::LayoutSetupService>(libraryHandle, registry);
        registerService<services::ILayoutService, services::LayoutService>(libraryHandle, registry);

        registerObjectFactory<ITrigger, triggers::TimeFrameTrigger>("TimeFrameTrigger"_id, libraryHandle, registry);

        registerObjectFactory<IController, controllers::ViewController>("ViewController"_id, libraryHandle, registry);
        registerObjectFactory<IController, controllers::FrameCounter>("FrameCounter"_id, libraryHandle, registry);
        registerObjectFactory<IController, controllers::Profiler>("Profiler"_id, libraryHandle, registry);
        registerObjectFactory<IController, controllers::EventDispatcher>("EventDispatcher"_id, libraryHandle, registry);
        registerObjectFactory<IController, controllers::StateUpdater>("StateUpdater"_id, libraryHandle, registry);
        registerObjectFactory<IController, controllers::AccelerationMovementController>("AccelerationMovementController"_id, libraryHandle, registry);
        registerObjectFactory<IController, controllers::MovementController>("MovementController"_id, libraryHandle, registry);
        registerObjectFactory<IController, controllers::TransformController>("TransformController"_id, libraryHandle, registry);
        registerObjectFactory<IController, controllers::ConsoleFlusher>("ConsoleFlusher"_id, libraryHandle, registry);
        registerObjectFactory<IController, controllers::WindowSystemController>("WindowSystemController"_id, libraryHandle, registry);

        registerObjectFactory<services::ISceneDirector, services::scene::directors::AssetLoader>("AssetLoader"_id, libraryHandle, registry);
        registerObjectFactory<services::ISceneDirector, services::scene::directors::ApplicationDirector>("ApplicationDirector"_id, libraryHandle, registry);
        registerObjectFactory<services::ISceneDirector, services::scene::directors::ViewDirector>("ViewDirector"_id, libraryHandle, registry);
    }
}