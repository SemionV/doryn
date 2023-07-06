#include "dependencies.h"

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

int main()
{
    std::cout << "dory:native test application" << std::endl;
    std::cout << "Press any key to process to render frame OR ESC to exit\r" << std::endl;

    dory::MouseMessage mouseMessage(1);
    dory::ConsoleMessage consoleMessage(2, -1, -1);

    checkMessage(mouseMessage);
    checkMessage(consoleMessage);

    dory::MessagePool inputMessagePool;
    dory::DataContext context;
    dory::Engine engine(context);
    
    test::TestController controller(inputMessagePool);
    engine.addController(&controller);

    dory::SystemConsole systemConsole;
    dory::SystemWindow systemWindow;
    dory::InputController inputController(inputMessagePool);
    inputController.addDevice(&systemConsole);
    inputController.addDevice(&systemWindow);
    engine.addController(&inputController);

    engine.initialize(context);

    dory::BasicFrameService frameService;
    frameService.startLoop(engine);

    std::cout << "Session is over." << std::endl;

    return 0;
}