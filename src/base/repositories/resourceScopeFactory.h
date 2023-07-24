#pragma once

#include "base/doryExport.h"
#include "base/resources/resourceScope.h"

namespace dory
{
    class DORY_API ResourceScopeFactory
    {
        private:
            int resourceIdCounter;

        public:
            ResourceScope createScope();

        private:
            int getNewResourceScopeId();
    };
}