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
        std::shared_ptr<extensionPlatform::IResourceHandle<ServicePtrType>> serviceHandle;

    public:
        void setServiceHandle(extensionPlatform::LibraryHandle libraryHandle, ServicePtrType service)
        {
            serviceHandle = extensionPlatform::makeResourceHandle<ServicePtrType>(libraryHandle, std::move(service));
        }

        void resetServiceHandle()
        {
            serviceHandle.reset();
        }

        auto getServiceReference()
        {
            if(serviceHandle)
            {
                return serviceHandle->lock();
            }

            return extensionPlatform::ResourceRef<ServicePtrType>{{}, nullptr};
        }
    };

    struct ServiceLayer: ServiceHandleController<services::ILibraryService>
    {
    public:
        std::shared_ptr<services::ILibraryService> libraryService;

    private:
        using FileServiceType = std::shared_ptr<services::IFileService>;
        std::shared_ptr<extensionPlatform::IResourceHandle<FileServiceType>> fileServiceHandle;

    public:
        void setFileService(extensionPlatform::LibraryHandle libraryHandle, FileServiceType fileService)
        {
            fileServiceHandle = extensionPlatform::makeResourceHandle<FileServiceType>(libraryHandle, std::move(fileService));
        }

        void setFileService(FileServiceType fileService)
        {
            fileServiceHandle = extensionPlatform::makeResourceHandle<FileServiceType>(std::move(fileService));
        }

        void resetFileService()
        {
            fileServiceHandle.reset();
        }

        extensionPlatform::ResourceRef<FileServiceType> getFileService()
        {
            if(fileServiceHandle)
            {
                return fileServiceHandle->lock();
            }

            return extensionPlatform::ResourceRef<FileServiceType>{{}, nullptr};
        }
    };

    struct Registry
    {
        EventLayer events;
        ServiceLayer services;
    };
}