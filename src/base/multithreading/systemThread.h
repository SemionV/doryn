#pragma once

#include "task.h"

namespace dory
{
    class DORY_API SystemThread
    {
        private:
            Task* regularTask;
            std::vector<Task*> irregularTasks;
            std::mutex mutex;
            bool isStop;

        public:
            explicit SystemThread(Task* regularTask);
            explicit SystemThread();
            ~SystemThread();
            virtual void invokeTask(Task* task);
            virtual void stop();
            virtual void run();

        private:
            virtual void threadMain();
            void invokeTaskSafe(Task* task);
    };
}