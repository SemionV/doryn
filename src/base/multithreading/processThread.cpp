#include "base/dependencies.h"
#include "processThread.h"

namespace dory
{
    void ProcessThread::invokeTaskSafe(std::shared_ptr<Task> task)
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