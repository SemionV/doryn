#include "dependencies.h"

#include <windows.h>

#include <thread>
#include <iostream>
#include <functional>
#include <mutex>

int runDory()
{
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

class TestTask: public dory::Task
{
    public:
        void operator()() override
        {
            std::cout << std::this_thread::get_id() << ": Irregular test task" << std::endl;
        }
};

class SleepThreadTask: public dory::Task
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
    /*TestTask testTask;
    SleepThreadTask threadTask;
    dory::SystemThread systemThread(&threadTask);

    systemThread.run();

    const std::chrono::milliseconds threadMainSleepInterval = std::chrono::milliseconds(3000);
    std::this_thread::sleep_for(threadMainSleepInterval);

    std::cout << std::this_thread::get_id() << ": main thread: schedule a task" << std::endl;
    systemThread.invokeTask(&testTask);
    std::cout << std::this_thread::get_id() << ": main thread: task should be invoked" << std::endl;

    systemThread.stop();*/

    return runDory();
}