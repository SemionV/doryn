#pragma once

#include "base/dependencies.h"
#include "base/doryExport.h"
#include "base/resources/resourceScope.h"
#include "base/messaging/event.h"

namespace dory
{
    class DORY_API ResourceScopeRepository
    {
        private:
            int resourceIdCounter;
            EventDispatcher<ResourceScope> onStartScope;
            EventDispatcher<ResourceScope> onFinishScope;

        public:
            ResourceScope createScope();
            void startScope(ResourceScope scope);
            void finishScope(ResourceScope scope);
            std::unique_ptr<Event<ResourceScope>> getOnStartScope();
            std::unique_ptr<Event<ResourceScope>> getOnFinishScope();

        private:
            int getNewResourceScopeId();
    };
}