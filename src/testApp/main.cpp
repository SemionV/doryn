#include "dependencies.h"

#include <windows.h>

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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
{
    return runDory();
}