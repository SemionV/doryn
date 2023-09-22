#include "base/dependencies.h"
#include "currentProcessThread.h"

namespace dory::multithreading
{
    void CurrentProcessThread::invokeTask(std::shared_ptr<Task> task)
    {
        task->reset();
        task->operator()();
    }

    void CurrentProcessThread::stop()
    {
    }


    void CurrentProcessThread::run()
    {        
    }
}