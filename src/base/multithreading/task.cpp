#include "task.h"

namespace dory
{
    Task::Task():
        isDoneFlag(false),
        isErrorFlag(false)
    {                
    }

    void Task::setDone(bool isDone)
    {
        isDoneFlag = isDone;
    }

    bool Task::getDone()
    {
        return isDoneFlag;
    }

    void Task::setError(bool isError)
    {
        isErrorFlag = isError;
    }

    bool Task::getError()
    {
        return isErrorFlag;
    }
}