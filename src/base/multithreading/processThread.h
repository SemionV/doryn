#pragma once

#include "task.h"

namespace dory
{
    class DORY_API ProcessThread
    {
        public:
            virtual void invokeTask(std::shared_ptr<Task> task) = 0;
            virtual void stop() = 0;
            virtual void run() = 0;
    };
}