#include "dependencies.h"

#include <windows.h>

#include <thread>
#include <iostream>
#include <functional>
#include <mutex>

void checkMessage(dory::Message& message)
{
    auto pMouseMessage = dynamic_cast<dory::MouseMessage*>(&message);
    auto pConsoleMessage = dynamic_cast<dory::ConsoleMessage*>(&message);

    if(pMouseMessage)
    {
        std::cout << "MouseMessage" << std::endl;
    }
    
    if(pConsoleMessage)
    {
        std::cout << "ConsoleMessage" << std::endl;
    }
}

int runDory()
{
    dory::MouseMessage mouseMessage(1);
    dory::ConsoleMessage consoleMessage(2, -1, -1);

    checkMessage(mouseMessage);
    checkMessage(consoleMessage);

    dory::MessagePool inputMessagePool;
    dory::DataContext context;
    dory::Engine engine(context);

    dory::InputController inputController(inputMessagePool);
    engine.addController(&inputController);

    dory::SystemConsole systemConsole;
    inputController.addDevice(&systemConsole);
    
    dory::SystemWindow systemWindow;
    inputController.addDevice(&systemWindow);

    test::TestController controller(inputMessagePool);
    engine.addController(&controller);

    engine.initialize(context);

    std::cout << "dory:native test application" << std::endl;
    std::cout << "Press any key to process to render frame OR ESC to exit\r" << std::endl;

    dory::BasicFrameService frameService;
    frameService.startLoop(engine);

    std::cout << "Session is over." << std::endl;

    return 0;
}

/*int main()
{
    return runDory();
}*/

class Task
{
    private:
        bool isDoneFlag;
        bool isErrorFlag;

    public:
        Task():
            isDoneFlag(false)
        {                
        }

        void setDone(bool isDone)
        {
            isDoneFlag = isDone;
        }

        bool getDone()
        {
            return isDoneFlag;
        }

        void setError(bool isError)
        {
            isErrorFlag = isError;
        }

        bool getError()
        {
            return isErrorFlag;
        }

        virtual void operator()() = 0;
};

class SystemThread
{
    private:
        Task* regularTask;
        std::vector<Task*> irregularTasks;
        std::mutex mutex;
        bool isStop;

    public:
        explicit SystemThread(Task* regularTask):
            regularTask(regularTask),
            irregularTasks()
        {
        }

        explicit SystemThread():
            regularTask(nullptr),
            irregularTasks()
        {
        }

        virtual void invokeTask(Task* task)
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

        virtual void stop()
        {
            isStop = true;
        }

        virtual void run()
        {
            std::thread workingThread = std::thread(&threadMain, this);
            workingThread.detach();
        }

    private:
        virtual void threadMain()
        {
            while(!isStop)
            {
                mutex.lock();
                std::size_t count = irregularTasks.size();
                for(std::size_t i = 0; i < count; i++)
                {
                    Task* task = irregularTasks[i];
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
                mutex.unlock();

                if(regularTask)
                {
                    regularTask->setDone(false);
                    regularTask->setError(false);

                    try
                    {
                        regularTask->operator()();
                    }
                    catch(const std::exception& e)
                    {
                        regularTask->setError(true);
                    }

                    regularTask->setDone(true);
                }
            }
        }
};

class TestTask: public Task
{
    public:
        void operator()() override
        {
            std::cout << std::this_thread::get_id() << ": Irregular test task" << std::endl;
        }
};

class SleepThreadTask: public Task
{
    public:
        void operator()() override
        {
            std::cout << std::this_thread::get_id() << ": Regular test task: sleep for 1 second" << std::endl;
            const std::chrono::milliseconds threadMainSleepInterval = std::chrono::milliseconds(1000);
            std::this_thread::sleep_for(threadMainSleepInterval);
        }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
{
    TestTask testTask;
    SleepThreadTask threadTask;
    SystemThread systemThread(&threadTask);

    systemThread.run();

    const std::chrono::milliseconds threadMainSleepInterval = std::chrono::milliseconds(3000);
    std::this_thread::sleep_for(threadMainSleepInterval);

    std::cout << std::this_thread::get_id() << ": main thread: schedule a task" << std::endl;
    systemThread.invokeTask(&testTask);
    std::cout << std::this_thread::get_id() << ": main thread: task should be invoked" << std::endl;

    systemThread.stop();

    return runDory();
}