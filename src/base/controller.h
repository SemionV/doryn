#pragma once

#include <chrono>

#include "doryExport.h"
#include "dataContext.h"
#include "types.h"

namespace dory
{
    class DORY_API Controller
    {
        public:
            Controller();
            virtual ~Controller();

            virtual void initialize(DataContext& context) = 0;
            virtual void stop(DataContext& context) = 0;
            virtual void update(const TimeStep& timeStep, DataContext& context) = 0;
    };
}