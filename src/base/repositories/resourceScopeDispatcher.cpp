#include "base/dependencies.h"
#include "resourceScopeDispatcher.h"

namespace dory
{
    void ResourceScopeDispatcher::startScope(ResourceScope scope)
    {
        onStartScope(scope);
    }

    void ResourceScopeDispatcher::finishScope(ResourceScope scope)
    {
        onFinishScope(scope);
    } 

    std::unique_ptr<Event<ResourceScope>> ResourceScopeDispatcher::getOnStartScope()
    {
        return std::unique_ptr<Event<ResourceScope>>(&onStartScope);
    }

    std::unique_ptr<Event<ResourceScope>> ResourceScopeDispatcher::getOnFinishScope()
    {
        return std::unique_ptr<Event<ResourceScope>>(&onFinishScope);
    }
}