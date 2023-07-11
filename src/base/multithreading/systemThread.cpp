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

    void SystemThread::invokeTask(Task* task)
    {
        task->setDone(false);
        task->setError(false);

        mutex.lock();
        irregularTasks.push_back(task);
        mutex.unlock();

        while(!task->getDone());

        mutex.lock();
        std::size_t count = irregularTasks.size();
        for(std::size_t i = 0; i < count; i++)
        {
            if(irregularTasks[i] == task)
            {
                irregularTasks.erase(irregularTasks.begin() + i);
                break;
            }
        }
        mutex.unlock();
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
            mutex.lock();
            std::size_t count = irregularTasks.size();
            for(std::size_t i = 0; i < count; i++)
            {
                Task* task = irregularTasks[i];
                invokeTaskSafe(task);
            }
            mutex.unlock();

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
        try
        {
            task->operator()();
        }
        catch(const std::exception& e)
        {
            task->setError(true);
        }

        task->setDone(true);
    }
}