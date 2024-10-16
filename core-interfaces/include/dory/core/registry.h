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
        events::mainController::EventHub mainControllerHub;
        events::mainController::EventDispatcher mainControllerDispatcher;
        events::application::EventHub applicationHub;
        events::application::EventDispatcher applicationDispatcher;
        events::io::EventHub standardIOHub;
        events::io::EventDispatcher standardIODispatcher;
        events::script::EventHub scriptHub;
        events::script::EventDispatcher scriptDispatcher;
        events::window::EventHub windowHub;
        events::window::EventDispatcher windowDispatcher;
    };

    struct ServiceLayer
    {
        std::shared_ptr<services::IFileService> fileService;
        std::shared_ptr<services::ILibraryService> libraryService;
    };

    struct Registry
    {
        EventLayer events;
        ServiceLayer services;
    };
}