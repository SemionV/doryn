#include "base/dependencies.h"
#include "currentProcessThread.h"

namespace dory
{
    void CurrentProcessThread::invokeTask(std::shared_ptr<Task> task)
    {
        task->setDone(false);
        task->setError(false);

        invokeTaskSafe(task);
    }

    void CurrentProcessThread::stop()
    {
    }


    void CurrentProcessThread::run()
    {        
    }
}