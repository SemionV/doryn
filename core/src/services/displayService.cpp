#include <dory/core/services/displayService.h>

namespace dory::core::services
{
    DisplayService::DisplayService(Registry& registry): DependencyResolver(registry)
    {}

    bool DisplayService::initializeDisplays()
    {
        
        return true;
    }
}
