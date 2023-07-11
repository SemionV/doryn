#pragma once

#include "base/multithreading/task.h"
#include "base/devices/systemConsole.h"

namespace dory
{
    class SystemConsole;

    class ReadConsoleInputTask: public Task
    {
        private:
            SystemConsole* systemConsole;

        public:
            ReadConsoleInputTask(SystemConsole* systemConsole);
            void operator()();
    };
}