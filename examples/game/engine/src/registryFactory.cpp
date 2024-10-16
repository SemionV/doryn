#include "dory/game/engine/registryFactory.h"
#include <dory/core/services/fileService.h>
#include <dory/core/services/libraryService.h>
#include <dory/core/events/scriptEvents.h>

namespace dory::game::engine
{
    std::shared_ptr<core::Registry> dory::game::engine::RegistryFactory::createRegistry() {
        auto registry = std::make_shared<core::Registry>();
        registry->services.fileService = std::make_shared<core::services::FileService>();
        registry->services.libraryService = std::make_shared<core::services::LibraryService>();

        auto scriptEvents = std::make_shared<core::events::script::EventDispatcher>();
        registry->events.scriptDispatcher = scriptEvents;
        registry->events.scriptHub = scriptEvents;

        return registry;
    }
}