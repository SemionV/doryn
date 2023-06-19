#pragma once

#include <chrono>

#include "doryExport.h"
#include "dataContext.h"

namespace dory
{
    class DORY_API Controller
    {
        public:
            Controller();
            virtual ~Controller();

            virtual void initialize(const DataContext& context) = 0;
            virtual void stop(const DataContext& context) = 0;
            virtual void update(const std::chrono::microseconds timeStep, DataContext& context) = 0;
    };
}