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
    
    /*dory::SystemWindow systemWindow;
    inputController.addDevice(&systemWindow);*/

    std::shared_ptr<doryWindows::WindowSystem> windowsSystem = std::make_shared<doryWindows::WindowSystem>();
    const doryWindows::WindowParameters windowParameters;
    windowsSystem->createWindow(windowParameters);
    inputController.addDevice(windowsSystem);

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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
{
    return runDory();
}