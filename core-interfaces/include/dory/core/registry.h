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
#include "resources/serviceIdentifer.h"
#include <dory/core/services/iSerializer.h>
#include <dory/generic/extension/registryResourceScope.h>
#include <dory/core/services/iDataFormatResolver.h>
#include <dory/core/services/iScriptService.h>
#include <dory/core/services/iConfigurationService.h>
#include <dory/core/services/iLocalizationService.h>
#include <dory/core/services/iFrameService.h>
#include <dory/core/services/iPipelineService.h>

namespace dory::core
{
    namespace repositories
    {
        using ICameraRepository = repositories::IRepository<resources::entity::Camera>;
        using IViewRepository = repositories::IRepository<resources::entity::View>;
        using IWindowRepository = repositories::IRepository<resources::entity::Window>;
    }

    template<typename TInterface, typename TIdentifier = resources::ServiceIdentifier>
    struct ServiceEntry
    {
        using InterfaceType = TInterface;
        using IdentifierType = TIdentifier;
    };

    struct Registry: public generic::registry::RegistryLayer<resources::ServiceIdentifier,
            /*Events*/
            ServiceEntry<events::pipeline::Bundle::IDispatcher>,
            ServiceEntry<events::pipeline::Bundle::IListener>,
            ServiceEntry<events::application::Bundle::IDispatcher>,
            ServiceEntry<events::application::Bundle::IListener>,
            ServiceEntry<events::io::Bundle::IDispatcher>,
            ServiceEntry<events::io::Bundle::IListener>,
            ServiceEntry<events::script::Bundle::IDispatcher>,
            ServiceEntry<events::script::Bundle::IListener>,
            ServiceEntry<events::window::Bundle::IDispatcher>,
            ServiceEntry<events::window::Bundle::IListener>,
            /*Devices*/
            ServiceEntry<devices::IStandardIODevice>,
            ServiceEntry<devices::ITerminalDevice>,
            /*Repositories*/
            ServiceEntry<repositories::ICameraRepository>,
            ServiceEntry<repositories::IViewRepository>,
            ServiceEntry<repositories::IWindowRepository>,
            ServiceEntry<repositories::IPipelineRepository>,
            /*Services*/
            ServiceEntry<services::ILibraryService>,
            ServiceEntry<services::IFileService>,
            ServiceEntry<services::IMultiSinkLogService, resources::Logger>,
            ServiceEntry<services::ILogService , resources::Logger>,
            ServiceEntry<services::serialization::ISerializer, resources::DataFormat>,
            ServiceEntry<services::IDataFormatResolver>,
            ServiceEntry<services::IScriptService>,
            ServiceEntry<services::IConfigurationService>,
            ServiceEntry<services::ILocalizationService>,
            ServiceEntry<services::IPipelineService>,
            ServiceEntry<services::IFrameService>>
    {};

    template<typename T>
    using RegistryResourceScope = generic::extension::RegistryResourceScope<generic::extension::RegistryResourceScopePolicy<T, Registry, resources::ServiceIdentifier>>;
}