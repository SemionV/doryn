#include "dependencies.h"

#include <windows.h>

#include <thread>
#include <iostream>
#include <functional>
#include <mutex>

int runDory()
{
    dory::DataContext context;
    dory::Engine engine(context);
    
    auto windowEventHub = std::make_shared<dory::SystemWindowEventHubDispatcher>();
    auto windowSystem = std::make_shared<doryWindows::WindowSystemParallel>(windowEventHub);

    auto consoleEventHub = std::make_shared<dory::SystemConsoleEventHubDispatcher>();
    auto consoleSystem = std::make_shared<doryWindows::ConsoleSystem>(consoleEventHub);

    dory::InputController inputController(consoleEventHub, windowEventHub);
    engine.addController(&inputController);
    inputController.addDevice(consoleSystem);
    inputController.addDevice(windowSystem);

    test::TestLogic logic(consoleEventHub, windowEventHub);

    engine.initialize(context);

    std::cout << "dory:native demo application" << std::endl;
    std::cout << "Press any key to process to render frame OR ESC to exit\r" << std::endl;

    const doryWindows::WindowParameters windowParameters;
    windowSystem->createWindow(windowParameters);

    dory::BasicFrameService frameService;
    frameService.startLoop(engine);

    std::cout << "Session is over." << std::endl;

    return 0;
}

/*int main()
{
    return runDory();
}*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
{
    return runDory();
}