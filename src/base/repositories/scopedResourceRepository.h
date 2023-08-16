#pragma once

#include "base/doryExport.h"
#include "base/resources/resourceScope.h"
#include "base/repositories/resourceScopeDispatcher.h"
namespace dory
{
    template<class T>
    class DORY_API ScopedResourceRepository
    {
        public:
            ScopedResourceRepository(std::shared_ptr<ResourceScopeDispatcher> resourceScopeDispatcher);

            std::unique_ptr<T> allocateResource(ResourceScope scope) = 0;
    };
}