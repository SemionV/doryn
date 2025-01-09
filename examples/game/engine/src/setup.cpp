#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "dory/game/setup.h"
#include "dory/core/resources/ecsType.h"
#include <dory/core/resources/assetType.h>

#ifdef DORY_PLATFORM_LINUX
#include <dory/core/devices/standardIoDeviceUnix.h>
#endif
#ifdef DORY_PLATFORM_WIN32
#include <dory/core/devices/standardIoDeviceWin32.h>
#endif
#include <dory/core/devices/terminalDevice.h>
#include <dory/core/devices/glfwWindowSystemDevice.h>
#include <dory/core/devices/openglGpuDevice.h>
#include <dory/core/devices/fileWatcherDevice.h>
#include <dory/core/devices/blockQueueStreamDevice.h>

#include <dory/core/repositories/pipelineRepository.h>
#include <dory/core/repositories/blockStreamRepository.h>
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
#include <dory/core/services/blockQueueService.h>
#include <dory/core/services/files/imageFileService.h>

namespace dory::game
{
    void Setup::setupRegistry(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
    {
        registerEventBundle<core::events::pipeline::Bundle>(libraryHandle, registry);
        registerEventBundle<core::events::application::Bundle>(libraryHandle, registry);
        registerEventBufferBundle<core::events::io::Bundle>(libraryHandle, registry);
        registerEventBundle<core::events::script::Bundle>(libraryHandle, registry);
        registerEventBufferBundle<core::events::window::Bundle>(libraryHandle, registry);
        registerEventBufferBundle<core::events::filesystem::Bundle>(libraryHandle, registry);
        registerEventBufferBundle<core::events::scene::Bundle>(libraryHandle, registry);

        registry.set<core::devices::IStandardIODevice>(libraryHandle, std::make_shared<core::devices::StandardIODevice>(registry));
        registry.set<core::devices::ITerminalDevice>(libraryHandle, std::make_shared<core::devices::TerminalDevice>(registry));
        registry.set<core::devices::IWindowSystemDevice, core::resources::WindowSystem::glfw>(libraryHandle, std::make_shared<core::devices::GlfwWindowSystemDevice>(registry));
        registry.set<core::devices::IFileWatcherDevice>(libraryHandle, std::make_shared<core::devices::FileWatcherDevice>(registry));
        registry.set<core::devices::IGpuDevice, core::resources::GraphicalSystem::opengl>(libraryHandle, std::make_shared<core::devices::OpenglGpuDevice>(registry));
        registry.set<core::devices::IBlockQueueStreamDevice>(libraryHandle, std::make_shared<core::devices::BlockQueueStreamDevice>(registry));

        registry.set<core::repositories::IBlockStreamRepository>(libraryHandle, std::make_shared<core::repositories::BlockStreamRepository>());
        registry.set<core::repositories::IViewRepository>(libraryHandle, std::make_shared<core::repositories::ViewRepository>());
        registry.set<core::repositories::IPipelineRepository>(libraryHandle, std::make_shared<core::repositories::PipelineRepository>());
        registerRepository<core::resources::entities::Camera>(libraryHandle, registry);
        auto sceneRepository = std::make_shared<core::repositories::EnttSceneRepository>();
        registry.set<core::repositories::ISceneRepository, core::resources::EcsType::entt>(libraryHandle, sceneRepository);
        registry.set<core::repositories::ISceneRepository>(libraryHandle, sceneRepository);

        auto windowRepository = std::make_shared<core::repositories::WindowRepository>();
        registry.set<core::repositories::IWindowRepository>(libraryHandle, windowRepository);
        registry.set<core::repositories::IWindowRepository, core::resources::WindowSystem::glfw>(libraryHandle, windowRepository);
        registry.set<core::repositories::IWindowRepository, core::resources::WindowSystem::glfw>(libraryHandle, windowRepository);

        registry.set<core::repositories::IGraphicalContextRepository>(libraryHandle, std::make_shared<core::repositories::GraphicalContextRepository>());
        registry.set<core::repositories::assets::IMeshRepository>(libraryHandle, std::make_shared<core::repositories::assets::MeshRepository>());
        registry.set<core::repositories::assets::IMaterialRepository>(libraryHandle, std::make_shared<core::repositories::assets::MaterialRepository>());
        registry.set<core::repositories::assets::IShaderRepository>(libraryHandle, std::make_shared<core::repositories::assets::ShaderRepository>());
        registry.set<core::repositories::bindings::IMeshBindingRepository, core::resources::GraphicalSystem::opengl>(libraryHandle, std::make_shared<core::repositories::bindings::MeshBindingRepository>());
        registry.set<core::repositories::bindings::IBufferBindingRepository, core::resources::GraphicalSystem::opengl>(libraryHandle, std::make_shared<core::repositories::bindings::BufferBindingRepository>());
        registry.set<core::repositories::bindings::IShaderBindingRepository, core::resources::GraphicalSystem::opengl>(libraryHandle, std::make_shared<core::repositories::bindings::ShaderBindingRepository>());
        registry.set<core::repositories::bindings::IMaterialBindingRepository, core::resources::GraphicalSystem::opengl>(libraryHandle, std::make_shared<core::repositories::bindings::MaterialBindingRepository>());

        registry.set<core::repositories::IShaderRepository, core::resources::GraphicalSystem::opengl>(libraryHandle, std::make_shared<core::repositories::ShaderRepository>());
        registry.set<core::repositories::IShaderProgramRepository, core::resources::GraphicalSystem::opengl>(libraryHandle, std::make_shared<core::repositories::ShaderProgramRepository>());

        registry.set<core::services::IFileService>(libraryHandle, std::make_shared<core::services::FileService>());
        registry.set<core::services::ILibraryService>(libraryHandle, std::make_shared<core::services::LibraryService>(registry));

        std::shared_ptr<core::services::IMultiSinkLogService> appLogger = std::make_shared<core::services::LogService>();
        registry.set<core::services::IMultiSinkLogService, core::resources::Logger::App>(libraryHandle, appLogger);
        registry.set<core::services::ILogService, core::resources::Logger::App>(libraryHandle, appLogger);
        registry.set<core::services::ILogService>(libraryHandle, appLogger);

        std::shared_ptr<core::services::IMultiSinkLogService> configLogger = std::make_shared<core::services::LogService>();
        registry.set<core::services::IMultiSinkLogService, core::resources::Logger::Config>(libraryHandle, configLogger);
        registry.set<core::services::ILogService, core::resources::Logger::Config>(libraryHandle, configLogger);

        registry.set<core::services::serialization::ISerializer, core::resources::DataFormat::yaml>(libraryHandle, std::make_shared<core::services::serialization::YamlSerializer>());
        registry.set<core::services::serialization::ISerializer, core::resources::DataFormat::json>(libraryHandle, std::make_shared<core::services::serialization::JsonSerializer>());
        registry.set<core::services::IDataFormatResolver>(libraryHandle, std::make_shared<core::services::DataFormatResolver>());

        registry.set<core::services::IScriptService>(libraryHandle, std::make_shared<core::services::ScriptService>(registry));
        registry.set<core::services::IConfigurationService>(libraryHandle, std::make_shared<core::services::ConfigurationService>(registry));
        registry.set<core::services::ILocalizationService>(libraryHandle, std::make_shared<core::services::LocalizationService>(registry));
        registry.set<core::services::IPipelineService>(libraryHandle, std::make_shared<core::services::PipelineService>(registry));
        registry.set<core::services::ILoopService>(libraryHandle, std::make_shared<core::services::LoopService>(registry));

        registry.set<core::services::IWindowService>(libraryHandle, std::make_shared<core::services::WindowService>(registry));
        registry.set<core::services::IViewService>(libraryHandle, std::make_shared<core::services::ViewService>(registry));

        registry.set<core::services::graphics::IRenderer>(libraryHandle, std::make_shared<core::services::graphics::Renderer>(registry));
        registry.set<core::services::graphics::IAssetBinder>(libraryHandle, std::make_shared<core::services::graphics::MeshAssetBinder>(registry), core::resources::AssetTypeName::mesh);
        registry.set<core::services::graphics::IAssetBinder>(libraryHandle, std::make_shared<core::services::graphics::MaterialAssetBinder>(registry), core::resources::AssetTypeName::material);
        registry.set<core::services::graphics::IAssetBinder>(libraryHandle, std::make_shared<core::services::graphics::ShaderAssetBinder>(registry), core::resources::AssetTypeName::shader);

        registry.set<core::services::IAssetTypeResolver>(libraryHandle, std::make_shared<core::services::AssetTypeResolver>());
        registry.set<core::services::IAssetReloadHandler>(libraryHandle, std::make_shared<core::services::loaders::ExtensionLoader>(registry), std::string{core::resources::AssetTypeName::extension});
        registry.set<core::services::generators::IMeshGenerator>(libraryHandle, std::make_shared<core::services::generators::MeshGenerator>());

        auto sceneService = std::make_shared<core::services::EnttSceneService>();
        registry.set<core::services::ISceneService>(libraryHandle, sceneService);
        registry.set<core::services::ISceneService, core::resources::EcsType::entt>(libraryHandle, sceneService);

        auto sceneQueryService = std::make_shared<core::services::EnttSceneQueryService>(registry);
        registry.set<core::services::ISceneQueryService, core::resources::EcsType::entt>(libraryHandle, sceneQueryService);

        registry.set<core::services::IBlockQueueService>(libraryHandle, std::make_shared<core::services::BlockQueueService>(registry));
        registry.set<core::services::files::IImageFileService, core::resources::AssetFileFormat::bmp>(libraryHandle, std::make_shared<core::services::files::BmpImageFileService>());
        registry.set<core::services::files::IImageFileService, core::resources::AssetFileFormat::png>(libraryHandle, std::make_shared<core::services::files::PngImageFileService>());
    }
}