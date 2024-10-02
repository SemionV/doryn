#include "dory/game/engine/registryFactory.h"
#include <dory/core/services/fileService.h>

namespace dory::game::engine
{
    std::unique_ptr<core::Registry> dory::game::engine::RegistryFactory::createRegistry() {
        auto registry = std::make_unique<core::Registry>();
        registry->services.fileService = std::make_shared<core::services::FileService>();

        return registry;
    }
}