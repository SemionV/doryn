#include "dory/game/engine/setup.h"
#include <dory/core/services/fileService.h>
#include <dory/core/services/libraryService.h>
#include <dory/core/events/scriptEvents.h>

namespace dory::game::engine
{
    void dory::game::engine::Setup::setupRegistry(const core::extensionPlatform::LibraryHandle& libraryHandle, core::Registry& registry)
    {
        registry.set<core::services::IFileService>(libraryHandle,std::make_shared<core::services::FileService>());
        registry.set<core::services::ILibraryService>(libraryHandle, std::make_shared<core::services::LibraryService>());

        auto scriptEvents = std::make_shared<core::events::script::EventDispatcher>();
        registry.events.scriptDispatcher = scriptEvents;
        registry.events.scriptHub = scriptEvents;
    }
}