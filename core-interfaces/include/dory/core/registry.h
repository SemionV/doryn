#pragma once

#include <memory>
#include "services/iFileService.h"
#include "services/iLibraryService.h"
#include "events/eventTypes.h"
#include "generic/typeTraits.h"
#include <dory/core/extensionPlatform/resourceHandle.h>
#include <dory/core/devices/iStandardIODevice.h>

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

    struct Registry: RegistryLayer<services::ILibraryService,
            services::IFileService,
            /*Events*/
            events::mainController::IDispatcher,
            events::mainController::IListener,
            events::application::IEventDispatcher,
            events::application::IEventHub,
            events::io::IEventDispatcher,
            events::io::IEventHub,
            events::script::IEventDispatcher,
            events::script::IEventHub,
            events::window::IEventDispatcher,
            events::window::IEventHub,
            /*Devices*/
            devices::IStandardIODevice>
    {};
}