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
            repositories::IRepository<resources::entity::Camera>,
            repositories::IRepository<resources::entity::View>,
            /*Services*/
            services::ILibraryService,
            services::IFileService>
    {};
}