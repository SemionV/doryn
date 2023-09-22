#include "base/dependencies.h"
#include "task.h"

namespace dory::multithreading
{
    void Task::setDone(bool isDone)
    {
        isDoneFlag = isDone;
    }

    void Task::setError(bool isError)
    {
        isErrorFlag = isError;
    }
    Task::Task():
        isDoneFlag(false),
        isErrorFlag(false)
    {                
    }

    bool Task::getDone()
    {
        return isDoneFlag;
    }

    bool Task::getError()
    {
        return isErrorFlag;
    }

    void Task::reset()
    {
        setError(false);
        setDone(false);
    }

    void Task::operator()()
    {
        try
        {
            invoke();
            setDone(true);
        }
        catch(...)
        {
            setError(true);
        }
    }
}