#include "base/dependencies.h"
#include "readConsoleInputTask.h"

namespace dory
{
    ReadConsoleInputTask::ReadConsoleInputTask(SystemConsole* systemConsole):
        systemConsole(systemConsole)
    {
    }

    void ReadConsoleInputTask::operator()()
    {
        int inputKey = getch();
        systemConsole->onInput(inputKey);
    }
}