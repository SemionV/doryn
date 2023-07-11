#include "base/dependencies.h"
#include "lambdaTask.h"

namespace dory
{
    void LambdaTask::operator()()
    {
        lambda();
    }

    LambdaTask::LambdaTask(std::function<void()> lambda):
        lambda(lambda)
    {
    }
}