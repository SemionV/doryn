#pragma once

#include <memory>
#include "services/iFileService.h"
#include "services/iLibraryService.h"
#include "events/eventTypes.h"

namespace dory::core
{
    namespace services
    {
        class ILibraryService;
    }

    struct EventLayer
    {
        /*events::mainController::EventDispatcher mainControllerDispatcher;
        events::mainController::EventHub& mainControllerHub = mainControllerDispatcher;

        events::application::EventDispatcher applicationDispatcher;
        events::application::EventHub& applicationHub = applicationDispatcher;

        events::io::EventDispatcher standardIODispatcher;
        events::io::EventHub& standardIOHub = standardIODispatcher;*/

        std::shared_ptr<events::script::IEventDispatcher> scriptDispatcher;
        std::shared_ptr<events::script::IEventHub> scriptHub;

        /*events::window::EventDispatcher windowDispatcher;
        events::window::EventHub& windowHub = windowDispatcher;*/
    };

    template<typename TService>
    class ServiceHandleController
    {
    private:
        using ServicePtrType = std::shared_ptr<TService>;
        std::shared_ptr<extensionPlatform::IResourceHandle<ServicePtrType>> _serviceHandle;

    protected:
        void _setServiceHandle(extensionPlatform::LibraryHandle libraryHandle, std::shared_ptr<TService> service)
        {
            _serviceHandle = extensionPlatform::makeResourceHandle<ServicePtrType>(libraryHandle, std::move(service));
        }

        void _resetServiceHandle()
        {
            _serviceHandle.reset();
        }

        auto _getServiceReference()
        {
            if(_serviceHandle)
            {
                return _serviceHandle->lock();
            }

            return extensionPlatform::ResourceRef<ServicePtrType>{{}, nullptr};
        }
    };

    template<typename... TServices>
    struct ServiceLayer: public ServiceHandleController<TServices>...
    {
    public:
        template<typename TService>
        void setService(extensionPlatform::LibraryHandle libraryHandle, std::shared_ptr<TService> service)
        {
            this->ServiceHandleController<TService>::_setServiceHandle(libraryHandle, service);
        }

        template<typename TService>
        void resetService()
        {
            ServiceHandleController<TService>::_resetServiceHandle();
        }

        template<typename TService>
        auto getService()
        {
            return ServiceHandleController<TService>::_getServiceReference();;
        }
    };

    struct Registry
    {
        EventLayer events;
        ServiceLayer<services::ILibraryService, services::IFileService> services;
    };
}