#pragma once

#include "dataContext.h"
#include "types.h"

namespace dory
{
    class DORY_API Controller
    {
        public:
            Controller();
            virtual ~Controller();

            virtual bool initialize(DataContext& context) = 0;
            virtual void stop(DataContext& context) = 0;
            virtual void update(const TimeSpan& timeStep, DataContext& context) = 0;
    };
}