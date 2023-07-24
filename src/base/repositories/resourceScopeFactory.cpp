#include "base/dependencies.h"
#include "resourceScopeFactory.h"

namespace dory
{
    ResourceScope ResourceScopeFactory::createScope()
    {
        ResourceScope resourceScope;
        resourceScope.id = getNewResourceScopeId();

        return resourceScope;
    }

    int ResourceScopeFactory::getNewResourceScopeId()
    {
        return resourceIdCounter++;
    }
}