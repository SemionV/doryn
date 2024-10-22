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
    namespace services
    {
        class ILibraryService;
    }

    template<typename TService>
    class ResourceHandleController
    {
    private:
        using ServicePtrType = std::shared_ptr<TService>;
        std::shared_ptr<extensionPlatform::ResourceHandle<ServicePtrType>> _serviceHandle;

    protected:
        void _set(extensionPlatform::LibraryHandle libraryHandle, std::shared_ptr<TService> service)
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

    template<typename... TServices>
    struct RegistryLayer: public ResourceHandleController<TServices>...
    {
    public:
        template<typename... TInterfaces, typename TImplementation = generic::FirstTypeT<TInterfaces...>>
        void set(extensionPlatform::LibraryHandle libraryHandle, std::shared_ptr<TImplementation> service)
        {
            (this->ResourceHandleController<TInterfaces>::_set(libraryHandle, service), ...);
        }

        template<typename TService>
        void reset()
        {
            ResourceHandleController<TService>::_reset();
        }

        template<typename TService>
        auto get()
        {
            return ResourceHandleController<TService>::_get();;
        }
    };

    namespace repositories
    {
        using ICameraRepository = repositories::IRepository<resources::entity::Camera>;
        using IViewRepository = repositories::IRepository<resources::entity::View>;
        using IWindowRepository = repositories::IRepository<resources::entity::Window>;
        using IPipelineNodeRepository = core::repositories::IRepository<core::resources::entity::PipelineNode<core::resources::DataContext>>;
    }

    struct Registry: RegistryLayer<
            /*Events*/
            events::mainController::Bundle::IDispatcher,
            events::mainController::Bundle::IListener,
            events::application::Bundle::IDispatcher,
            events::application::Bundle::IListener,
            events::io::Bundle::IDispatcher,
            events::io::Bundle::IListener,
            events::script::Bundle::IDispatcher,
            events::script::Bundle::IListener,
            events::window::Bundle::IDispatcher,
            events::window::Bundle::IListener,
            /*Devices*/
            devices::IStandardIODevice,
            devices::ITerminalDevice,
            /*Repositories*/
            repositories::ICameraRepository,
            repositories::IViewRepository,
            repositories::IWindowRepository,
            repositories::IPipelineRepository,
            repositories::IPipelineNodeRepository,
            /*Services*/
            services::ILibraryService,
            services::IFileService,
            services::ILogService,
            services::IAppLogger,
            services::IConfigLogger>
    {};
}