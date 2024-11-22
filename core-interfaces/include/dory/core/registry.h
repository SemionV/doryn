#pragma once

#include <map>
#include <dory/generic/registryLayer.h>
#include "services/iFileService.h"
#include "services/iLibraryService.h"
#include "events/eventTypes.h"
#include <dory/core/resources/dataFormat.h>
#include <dory/core/resources/logger.h>
#include <dory/generic/extension/resourceHandle.h>
#include <dory/core/devices/iStandardIODevice.h>
#include <dory/core/devices/iTerminalDevice.h>
#include <dory/core/repositories/iRepository.h>
#include <dory/core/repositories/iPipelineRepository.h>
#include "services/iLogService.h"
#include <dory/core/services/iSerializer.h>
#include <dory/generic/extension/registryResourceScope.h>
#include <dory/core/services/iDataFormatResolver.h>
#include <dory/core/services/iScriptService.h>
#include <dory/core/services/iConfigurationService.h>
#include <dory/core/services/iLocalizationService.h>
#include <dory/core/services/iFrameService.h>
#include <dory/core/services/iPipelineService.h>
#include <dory/core/devices/iWindowSystemDevice.h>
#include <dory/core/resources/windowSystem.h>
#include <dory/core/services/iWindowService.h>
#include <dory/core/services/iGraphicalSystem.h>
#include <dory/core/resources/graphicalSystem.h>
#include <dory/core/services/graphics/iRenderer.h>
#include <dory/core/repositories/iViewRepository.h>
#include <dory/core/devices/iFileWatcherDevice.h>
#include <dory/core/services/iAssetTypeResolver.h>
#include <dory/core/services/iAssetReloadHandler.h>
#include <dory/core/services/iAssetService.h>
#include <dory/core/services/graphics/iShaderService.h>
#include <dory/core/services/iSceneService.h>
#include <dory/core/services/iGraphicContextService.h>
#include <dory/core/services/graphics/iGraphicalContextService.h>
#include <dory/core/services/iViewService.h>
#include <dory/core/repositories/iWindowRepository.h>
#include <dory/core/repositories/iShaderRepository.h>
#include <dory/core/repositories/iShaderProgramRepository.h>
#include <dory/core/repositories/assets/iMeshRepository.h>
#include <dory/core/repositories/iSceneRepository.h>
#include <dory/core/repositories/iGraphicalContextRepository.h>
#include <dory/core/repositories/assets/iMeshRepository.h>
#include <dory/core/repositories/bindings/iMeshBindingRepository.h>
#include <dory/core/repositories/bindings/iBufferBindingRepository.h>
#include <dory/core/services/graphics/iGpuDriver.h>

#include <dory/core/resources/entities/camera.h>

namespace dory::core
{
    namespace repositories
    {
        using ICameraRepository = repositories::IRepository<resources::entities::Camera>;
    }

    class Registry: public generic::registry::RegistryLayer<
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
            generic::registry::ServiceEntry<events::filesystem::Bundle::IDispatcher>,
            generic::registry::ServiceEntry<events::filesystem::Bundle::IListener>,
            /*Devices*/
            generic::registry::ServiceEntry<devices::IStandardIODevice>,
            generic::registry::ServiceEntry<devices::ITerminalDevice>,
            generic::registry::ServiceEntry<devices::IWindowSystemDevice, resources::WindowSystem>,
            generic::registry::ServiceEntry<devices::IFileWatcherDevice>,
            /*Repositories*/
            generic::registry::ServiceEntry<repositories::ICameraRepository>,
            generic::registry::ServiceEntry<repositories::IViewRepository>,
            generic::registry::ServiceEntry<repositories::IWindowRepository>,
            generic::registry::ServiceEntry<repositories::IWindowRepository, resources::WindowSystem>,
            generic::registry::ServiceEntry<repositories::IPipelineRepository>,
            generic::registry::ServiceEntry<repositories::assets::IMeshRepository>,
            generic::registry::ServiceEntry<repositories::IShaderRepository, resources::GraphicalSystem>,
            generic::registry::ServiceEntry<repositories::IShaderProgramRepository, resources::GraphicalSystem>,
            generic::registry::ServiceEntry<repositories::IGraphicalContextRepository, resources::GraphicalSystem>,
            generic::registry::ServiceEntry<repositories::bindings::IMeshBindingRepository, resources::GraphicalSystem>,
            generic::registry::ServiceEntry<repositories::bindings::IBufferBindingRepository, resources::GraphicalSystem>,
            /*Services*/
            generic::registry::ServiceEntry<services::ILibraryService>,
            generic::registry::ServiceEntry<services::IFileService>,
            generic::registry::ServiceEntry<services::IMultiSinkLogService, resources::Logger>,
            generic::registry::ServiceEntry<services::ILogService>,
            generic::registry::ServiceEntry<services::ILogService, resources::Logger>,
            generic::registry::ServiceEntry<services::serialization::ISerializer, resources::DataFormat>,
            generic::registry::ServiceEntry<services::IDataFormatResolver>,
            generic::registry::ServiceEntry<services::IScriptService>,
            generic::registry::ServiceEntry<services::IConfigurationService>,
            generic::registry::ServiceEntry<services::ILocalizationService>,
            generic::registry::ServiceEntry<services::IPipelineService>,
            generic::registry::ServiceEntry<services::IFrameService>,
            generic::registry::ServiceEntry<services::IGraphicalSystem>,
            generic::registry::ServiceEntry<services::graphics::IRenderer, resources::GraphicalSystem>,
            generic::registry::ServiceEntry<services::graphics::IShaderService, resources::GraphicalSystem>,
            generic::registry::ServiceEntry<services::IViewService>,
            generic::registry::ServiceEntry<services::IWindowService>,//default window service
            generic::registry::ServiceEntry<services::IWindowService, resources::WindowSystem>,
            generic::registry::ServiceEntry<services::IAssetTypeResolver>,
            generic::registry::ServiceEntry<services::IAssetReloadHandler, std::string>,
            generic::registry::ServiceEntry<services::IAssetService>,
            generic::registry::ServiceEntry<services::ISceneService>,
            generic::registry::ServiceEntry<services::ISceneService, resources::EcsType>,
            generic::registry::ServiceEntry<services::IGraphicContextService>,
            generic::registry::ServiceEntry<services::graphics::IGpuDriver, resources::GraphicalSystem>>
    {};

    template<typename T>
    using RegistryResourceScope = generic::extension::RegistryResourceScope<T, Registry>;

    template<typename T, typename TIdentifier>
    using RegistryResourceScopeWithId = generic::extension::RegistryResourceScope<T, Registry, TIdentifier>;

}