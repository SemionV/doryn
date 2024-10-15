#include "dory/game/engine/registryFactory.h"
#include <dory/core/services/fileService.h>
#include <dory/core/services/libraryService.h>

namespace dory::game::engine
{
    std::shared_ptr<core::Registry> dory::game::engine::RegistryFactory::createRegistry() {
        auto registry = std::make_shared<core::Registry>();
        registry->services.fileService = std::make_shared<core::services::FileService>();
        registry->services.libraryService = std::make_shared<core::services::LibraryService>();

        return registry;
    }
}