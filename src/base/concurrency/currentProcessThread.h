#pragma once

#include "base/dependencies.h"
#include "task.h"
#include "processThread.h"

namespace dory::concurrency
{
    class CurrentProcessThread: public ProcessThread
    {
    public:
        void invokeTask(std::shared_ptr<Task> task) override
        {
            task->reset();
            task->operator()();
        }

        void stop() override
        {
        }

        void run() override
        {
        }
    };
}