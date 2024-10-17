#include "dory/game/engine/registryFactory.h"
#include <dory/core/services/fileService.h>
#include <dory/core/services/libraryService.h>
#include <dory/core/events/scriptEvents.h>

namespace dory::game::engine
{
    core::Registry dory::game::engine::RegistryFactory::createRegistry(const core::extensionPlatform::LibraryHandle& libraryHandle) {
        auto registry = core::Registry{};

        registry.services.setService<core::services::IFileService>(libraryHandle, std::make_shared<core::services::FileService>());
        registry.services.setService<core::services::ILibraryService>(libraryHandle, std::make_shared<core::services::LibraryService>());

        auto scriptEvents = std::make_shared<core::events::script::EventDispatcher>();
        registry.events.scriptDispatcher = scriptEvents;
        registry.events.scriptHub = scriptEvents;

        return registry;
    }
}