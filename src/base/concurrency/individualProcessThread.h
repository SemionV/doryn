#pragma once

#include <utility>

#include "task.h"
#include "processThread.h"

namespace dory::concurrency
{
    class IndividualProcessThread: public ProcessThread
    {
    private:
        std::shared_ptr<Task> regularTask;
        std::queue<std::shared_ptr<Task>> irregularTasks;
        std::mutex mutex;
        bool isStop;

    public:
        explicit IndividualProcessThread(std::shared_ptr<Task> regularTask):
                regularTask(std::move(regularTask)),
                irregularTasks(),
                isStop(false)
        {}

        IndividualProcessThread():
                regularTask(nullptr),
                irregularTasks(),
                isStop(false)
        {}

        ~IndividualProcessThread() override
        {
            isStop = true;
        }

        void invokeTask(std::shared_ptr<Task> task) override
        {
            {
                task->reset();

                const std::lock_guard<std::mutex> lock(mutex);
                irregularTasks.push(task);
            }

            while(!task->getDone() && !task->getError());
        }

        void setRegularTask(std::shared_ptr<Task> task)
        {
            regularTask = std::move(task);
        }

        void stop() override
        {
            isStop = true;
        }

        void run() override
        {
            std::thread workingThread = std::thread(&IndividualProcessThread::threadMain, this);
            workingThread.detach();
        }

    private:
        virtual void threadMain()
        {
            while(!isStop)
            {
                {
                    const std::lock_guard<std::mutex> lock(mutex);

                    while(!irregularTasks.empty())
                    {
                        std::shared_ptr<Task> task = irregularTasks.front();
                        task->operator()();
                        irregularTasks.pop();
                    }
                }

                if(regularTask)
                {
                    regularTask->reset();

                    regularTask->operator()();
                }
            }
        }
    };
}