#include "dory/game/engine/setup.h"
#include <dory/core/services/fileService.h>
#include <dory/core/services/libraryService.h>
#include <dory/core/events/mainControllerEvents.h>
#include <dory/core/events/applicationEvents.h>
#include <dory/core/events/ioEvents.h>
#include <dory/core/events/scriptEvents.h>
#include <dory/core/events/windowEvents.h>

#ifdef DORY_PLATFORM_LINUX
#include <dory/core/devices/standardIoDeviceUnix.h>
#endif

#ifdef DORY_PLATFORM_WIN32
#include <dory/core/devices/standardIoDeviceWin32.h>
#endif

namespace dory::game::engine
{
    void dory::game::engine::Setup::setupRegistry(const core::extensionPlatform::LibraryHandle& libraryHandle, core::Registry& registry)
    {
        registry.set<core::services::IFileService>(libraryHandle, std::make_shared<core::services::FileService>());
        registry.set<core::services::ILibraryService>(libraryHandle, std::make_shared<core::services::LibraryService>());

        registry.set<core::events::mainController::IEventDispatcher, core::events::mainController::IEventHub>(libraryHandle,
                std::make_shared<core::events::mainController::EventDispatcher>());

        registry.set<core::events::application::IEventDispatcher, core::events::application::IEventHub>(libraryHandle,
                std::make_shared<core::events::application::EventDispatcher>());

        registry.set<core::events::io::IEventDispatcher, core::events::io::IEventHub>(libraryHandle,
                std::make_shared<core::events::io::EventDispatcher>());

        registry.set<core::events::script::IEventDispatcher, core::events::script::IEventHub>(libraryHandle,
                std::make_shared<core::events::script::EventDispatcher>());

        registry.set<core::events::window::IEventDispatcher, core::events::window::IEventHub>(libraryHandle,
                std::make_shared<core::events::window::EventDispatcher>());

        auto ioEventDispatcher = registry.get<core::events::io::IEventDispatcher>();
        if(ioEventDispatcher)
        {
            registry.set<core::devices::IStandardIODevice>(libraryHandle, std::make_shared<core::devices::StandardIODevice>(*ioEventDispatcher));
        }
    }
}