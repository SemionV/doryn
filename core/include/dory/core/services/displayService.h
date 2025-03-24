#pragma once

#include <dory/core/dependencyResolver.h>
#include <dory/core/services/iDisplayService.h>

namespace dory::core::services
{
    class DisplayService: public IDisplayService, public DependencyResolver
    {
    public:
        explicit DisplayService(Registry& registry);

        bool initializeDisplays() final;
    };
}
