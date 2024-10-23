#pragma once

#include <memory>
#include "services/iFileService.h"
#include "services/iLibraryService.h"
#include "events/eventTypes.h"
#include "generic/typeTraits.h"
#include <dory/core/resources/entity.h>
#include <dory/core/extensionPlatform/resourceHandle.h>
#include <dory/core/devices/iStandardIODevice.h>
#include <dory/core/devices/iTerminalDevice.h>
#include <dory/core/repositories/iRepository.h>
#include <dory/core/repositories/iPipelineRepository.h>
#include "services/iLogService.h"

namespace dory::core
{
    enum class Identifier
    {
        General
    };

    template<typename TServiceInterface, auto identifier = Identifier::General>
    class ResourceHandleController
    {
    private:
        using ServicePtrType = std::shared_ptr<TServiceInterface>;
        std::shared_ptr<extensionPlatform::ResourceHandle<ServicePtrType>> _serviceHandle;

    protected:
        void _set(extensionPlatform::LibraryHandle libraryHandle, std::shared_ptr<TServiceInterface> service)
        {
            _serviceHandle = extensionPlatform::makeResourceHandle<ServicePtrType>(libraryHandle, std::move(service));
        }

        void _reset()
        {
            _serviceHandle.reset();
        }

        auto _get()
        {
            if(_serviceHandle)
            {
                return _serviceHandle->lock();
            }

            return extensionPlatform::ResourceRef<ServicePtrType>{{}, nullptr};
        }
    };

    template<typename TInterface, auto Identifier = Identifier::General>
    struct ServiceEntry
    {
        using InterfaceType = TInterface;
        static const constexpr decltype(Identifier) identifier = Identifier;
    };

    template<typename... TServices>
    struct RegistryLayer: public ResourceHandleController<typename TServices::InterfaceType, TServices::identifier>...
    {
    public:
        template<typename TInterface, auto identifier = Identifier::General>
        void set(extensionPlatform::LibraryHandle libraryHandle, std::shared_ptr<TInterface> service)
        {
            this->ResourceHandleController<TInterface, identifier>::_set(libraryHandle, service);
        }

        template<typename TService, auto identifier = Identifier::General>
        void reset()
        {
            this->ResourceHandleController<TService, identifier>::_reset();
        }

        template<typename TService, auto identifier = Identifier::General>
        auto get()
        {
            return this->ResourceHandleController<TService, identifier>::_get();;
        }
    };

    namespace repositories
    {
        using ICameraRepository = repositories::IRepository<resources::entity::Camera>;
        using IViewRepository = repositories::IRepository<resources::entity::View>;
        using IWindowRepository = repositories::IRepository<resources::entity::Window>;
        using IPipelineNodeRepository = core::repositories::IRepository<core::resources::entity::PipelineNode<core::resources::DataContext>>;
    }

    enum class Logger
    {
        App,
        Config
    };

    struct Registry: public RegistryLayer<
            /*Events*/
            ServiceEntry<events::mainController::Bundle::IDispatcher>,
            ServiceEntry<events::mainController::Bundle::IListener>,
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
            ServiceEntry<repositories::IPipelineNodeRepository>,
            /*Services*/
            ServiceEntry<services::ILibraryService>,
            ServiceEntry<services::IFileService>,
            ServiceEntry<services::IMultiSinkLogService, Logger::App>,
            ServiceEntry<services::IMultiSinkLogService, Logger::Config>>
    {};
}