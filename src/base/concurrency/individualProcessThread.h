#pragma once

#include "task.h"
#include "processThread.h"

namespace dory::concurrency
{
    class DORY_API IndividualProcessThread: public ProcessThread
    {
        private:
            std::shared_ptr<Task> regularTask;
            std::queue<std::shared_ptr<Task>> irregularTasks;
            std::mutex mutex;
            bool isStop;

        public:
            explicit IndividualProcessThread(std::shared_ptr<Task> regularTask);
            explicit IndividualProcessThread();
            ~IndividualProcessThread();
            void invokeTask(std::shared_ptr<Task> task) override;
            void setRegularTask(std::shared_ptr<Task> task);
            void stop() override;
            void run() override;

        private:
            virtual void threadMain();
    };
}