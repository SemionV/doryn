#include "base/dependencies.h"
#include "individualProcessThread.h"

namespace dory
{
    IndividualProcessThread::IndividualProcessThread(std::shared_ptr<Task> regularTask):
        regularTask(regularTask),
        irregularTasks(),
        isStop(false)
    {
    }

    IndividualProcessThread::IndividualProcessThread():
        regularTask(nullptr),
        irregularTasks(),
        isStop(false)
    {
    }

    IndividualProcessThread::~IndividualProcessThread()
    {
        isStop = true;
    }

    void IndividualProcessThread::invokeTask(std::shared_ptr<Task> task)
    {
        {
            task->setDone(false);
            task->setError(false);

            const std::lock_guard<std::mutex> lock(mutex);
            irregularTasks.push(task);
        }

        while(!task->getDone() && !task->getError());
    }

    void IndividualProcessThread::stop()
    {
        isStop = true;
    }

    void IndividualProcessThread::run()
    {
        std::thread workingThread = std::thread(&threadMain, this);
        workingThread.detach();
    }

    void IndividualProcessThread::threadMain()
    {
        while(!isStop)
        {
            {
                const std::lock_guard<std::mutex> lock(mutex);

                while(!irregularTasks.empty())
                {
                    std::shared_ptr<Task> task = irregularTasks.front();
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
}