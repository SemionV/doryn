#pragma once

#include "base/dataContext.h"
#include "base/types.h"
#include "entity.h"

namespace dory
{
    class DORY_API Controller
    {
        public:
            virtual bool initialize(domain::entity::IdType referenceId, DataContext& context) = 0;
            virtual void stop(domain::entity::IdType referenceId, DataContext& context) = 0;
            virtual void update(domain::entity::IdType referenceId, const TimeSpan& timeStep, DataContext& context) = 0;
    };
}