#include "base/dependencies.h"
#include "systemThread.h"

namespace dory
{
    SystemThread::SystemThread(Task* regularTask):
        regularTask(regularTask),
        irregularTasks(),
        isStop(false)
    {
    }

    SystemThread::SystemThread():
        regularTask(nullptr),
        irregularTasks(),
        isStop(false)
    {
    }

    SystemThread::~SystemThread()
    {
        isStop = true;
    }

    void SystemThread::invokeTask(Task* task)
    {
        {
            task->setDone(false);
            task->setError(false);

            const std::lock_guard<std::mutex> lock(mutex);
            irregularTasks.push(task);
        }

        while(!task->getDone() && !task->getError());
    }

    void SystemThread::stop()
    {
        isStop = true;
    }

    void SystemThread::run()
    {
        std::thread workingThread = std::thread(&threadMain, this);
        workingThread.detach();
    }

    void SystemThread::threadMain()
    {
        while(!isStop)
        {
            {
                const std::lock_guard<std::mutex> lock(mutex);

                while(irregularTasks.size() > 0)
                {
                    Task* task = irregularTasks.front();
                    invokeTaskSafe(task);
                    irregularTasks.pop();
                }
            }

            if(regularTask)
            {
                regularTask->setDone(false);
                regularTask->setError(false);

                invokeTaskSafe(regularTask);
            }
        }
    }

    void SystemThread::invokeTaskSafe(Task* task)
    {
        if(task)
        {
            try
            {
                task->operator()();
                task->setDone(true);
            }
            catch(...)
            {
                task->setError(true);
            }
        }
    }
}