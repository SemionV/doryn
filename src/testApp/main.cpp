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
    auto resourceScopeFactory = std::make_shared<dory::ResourceScopeFactory>();
    auto resourceScopeDispatcher = std::make_shared<dory::ResourceScopeDispatcher>();
    dory::ResourceScope updateControllersScope = resourceScopeFactory->createScope();
    dory::Engine engine(context, resourceScopeDispatcher, updateControllersScope);
    auto deviceListener = std::make_shared<dory::DeviceListener>();
    auto windowSystem = std::make_shared<doryWindows::WindowSystemParallel>();
    windowSystem->attachListener(deviceListener);

    auto consoleEventHub = std::make_shared<dory::SystemConsoleEventHubDispatcher>();
    auto consoleSystem = std::make_shared<doryWindows::ConsoleSystem>(consoleEventHub);
    consoleSystem->attachListener(deviceListener);

    dory::InputController inputController(inputMessagePool, consoleEventHub);
    engine.addController(&inputController);

    test::TestLogic logic(consoleEventHub);

    inputController.addDevice(consoleSystem);
    inputController.addDevice(windowSystem);
    inputController.addDeviceListener(deviceListener);

    test::TestController controller(inputMessagePool);
    engine.addController(&controller);

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