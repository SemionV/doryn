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
        events::mainController::EventDispatcher mainControllerDispatcher;
        events::mainController::EventHub& mainControllerHub = mainControllerDispatcher;

        events::application::EventDispatcher applicationDispatcher;
        events::application::EventHub& applicationHub = applicationDispatcher;

        events::io::EventDispatcher standardIODispatcher;
        events::io::EventHub& standardIOHub = standardIODispatcher;

        std::shared_ptr<events::script::IEventDispatcher> scriptDispatcher;
        std::shared_ptr<events::script::IEventHub> scriptHub;

        events::window::EventDispatcher windowDispatcher;
        events::window::EventHub& windowHub = windowDispatcher;
    };

    struct ServiceLayer
    {
    private:
        using FileServiceType = std::shared_ptr<services::IFileService>;
        std::shared_ptr<extensionPlatform::IResourceHandle<FileServiceType>> fileServiceHandle;

    public:
        void setFileService(extensionPlatform::LibraryHandle libraryHandle, FileServiceType fileServices)
        {
            fileServiceHandle = extensionPlatform::makeResourceHandle<FileServiceType>(libraryHandle, std::move(fileServices));
        }

        void setFileService(FileServiceType fileServices)
        {
            fileServiceHandle = extensionPlatform::makeResourceHandle<FileServiceType>(std::move(fileServices));
        }

        extensionPlatform::ResourceRef<FileServiceType> getFileService()
        {
            return fileServiceHandle->lock();
        }

        std::shared_ptr<services::IFileService> fileService;
        std::shared_ptr<services::ILibraryService> libraryService;
    };

    struct Registry
    {
        EventLayer events;
        ServiceLayer services;
    };
}