#include "base/dependencies.h"
#include "resourceScopeRepository.h"

namespace dory
{
    ResourceScope ResourceScopeRepository::createScope()
    {
        ResourceScope resourceScope;
        resourceScope.id = getNewResourceScopeId();

        return resourceScope;
    }

    int ResourceScopeRepository::getNewResourceScopeId()
    {
        return resourceIdCounter++;
    }

    void ResourceScopeRepository::startScope(ResourceScope scope)
    {
        onStartScope(scope);
    }

    void ResourceScopeRepository::finishScope(ResourceScope scope)
    {
        onFinishScope(scope);
    } 

    std::unique_ptr<Event<ResourceScope>> ResourceScopeRepository::getOnStartScope()
    {
        return std::unique_ptr<Event<ResourceScope>>(&onStartScope);
    }

    std::unique_ptr<Event<ResourceScope>> ResourceScopeRepository::getOnFinishScope()
    {
        return std::unique_ptr<Event<ResourceScope>>(&onFinishScope);
    }
}