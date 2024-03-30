#pragma once

#include "task.h"

namespace dory::concurrency
{
    class ProcessThread
    {
        public:
            virtual ~ProcessThread() = default;

            virtual void invokeTask(std::shared_ptr<Task> task) = 0;
            virtual void stop() = 0;
            virtual void run() = 0;
    };
}