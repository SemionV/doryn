#pragma once

#include "task.h"
#include "processThread.h"

namespace dory::concurrency
{
    class DORY_API CurrentProcessThread: public ProcessThread
    {
        public:
            void invokeTask(std::shared_ptr<Task> task) override;
            void stop() override;
            void run() override;
    };
}