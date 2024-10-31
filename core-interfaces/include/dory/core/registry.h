#pragma once

#include <map>
#include <dory/generic/registryLayer.h>
#include "services/iFileService.h"
#include "services/iLibraryService.h"
#include "events/eventTypes.h"
#include <dory/core/resources/entity.h>
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

namespace dory::core
{
    namespace repositories
    {
        using ICameraRepository = repositories::IRepository<resources::entity::Camera>;
        using IViewRepository = repositories::IRepository<resources::entity::View>;
        using IWindowRepository = repositories::IRepository<resources::entity::Window>;
    }

    struct Registry: public generic::registry::RegistryLayer<
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
            /*Devices*/
            generic::registry::ServiceEntry<devices::IStandardIODevice>,
            generic::registry::ServiceEntry<devices::ITerminalDevice>,
            generic::registry::ServiceEntry<devices::IWindowSystemDevice, resources::WindowSystem>,
            /*Repositories*/
            generic::registry::ServiceEntry<repositories::ICameraRepository>,
            generic::registry::ServiceEntry<repositories::IViewRepository>,
            generic::registry::ServiceEntry<repositories::IWindowRepository>,
            generic::registry::ServiceEntry<repositories::IPipelineRepository>,
            /*Services*/
            generic::registry::ServiceEntry<services::ILibraryService>,
            generic::registry::ServiceEntry<services::IFileService>,
            generic::registry::ServiceEntry<services::IMultiSinkLogService, resources::Logger>,
            generic::registry::ServiceEntry<services::ILogService , resources::Logger>,
            generic::registry::ServiceEntry<services::serialization::ISerializer, resources::DataFormat>,
            generic::registry::ServiceEntry<services::IDataFormatResolver>,
            generic::registry::ServiceEntry<services::IScriptService>,
            generic::registry::ServiceEntry<services::IConfigurationService>,
            generic::registry::ServiceEntry<services::ILocalizationService>,
            generic::registry::ServiceEntry<services::IPipelineService>,
            generic::registry::ServiceEntry<services::IFrameService>>
    {};

    template<typename T>
    using RegistryResourceScope = generic::extension::RegistryResourceScope<generic::extension::RegistryResourceScopePolicy<T, Registry, generic::registry::ServiceIdentifier>>;
}