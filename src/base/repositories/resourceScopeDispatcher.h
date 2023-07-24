#pragma once

#include "base/doryExport.h"
#include "base/resources/resourceScope.h"
#include "base/messaging/event.h"

namespace dory
{
    class DORY_API ResourceScopeDispatcher
    {
        private:
            EventDispatcher<ResourceScope> onStartScope;
            EventDispatcher<ResourceScope> onFinishScope;

        public:
            void startScope(ResourceScope scope);
            void finishScope(ResourceScope scope);
            std::unique_ptr<Event<ResourceScope>> getOnStartScope();
            std::unique_ptr<Event<ResourceScope>> getOnFinishScope();
    };
}