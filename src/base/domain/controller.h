#pragma once

#include "base/dataContext.h"
#include "base/types.h"
#include "entity.h"

namespace dory
{
    class DORY_API Controller
    {
        public:
            virtual bool initialize(DataContext& context) = 0;
            virtual void stop(DataContext& context) = 0;
            virtual void update(const int referenceId, const TimeSpan& timeStep, DataContext& context) = 0;
    };
}