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

    auto consoleSystem = std::make_shared<doryWindows::ConsoleSystem>();
    consoleSystem->attachListener(deviceListener);

    dory::InputController inputController(inputMessagePool);
    engine.addController(&inputController);

    inputController.addDevice(consoleSystem);
    inputController.addDevice(windowSystem);
    inputController.addDeviceListener(deviceListener);

    test::TestController controller(inputMessagePool);
    engine.addController(&controller);

    engine.initialize(context);

    std::cout << "dory:native test application" << std::endl;
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
    dory::Message* deletedMessage;
    auto deleter = [&](dory::Message* message)
    {
        std::cout << "delete Message " << message << std::endl;
        deletedMessage = message;
        delete message;
    };

    {
        dory::Message* message = new dory::Message(dory::MessageType::MouseTestMessage);
        std::unique_ptr<dory::Message, decltype(deleter)> messagePtr(message, deleter);
        //std::shared_ptr<dory::Message> messagePtrCopy(messagePtr.get());
        dory::Message messageCopy = *messagePtr;
    }

    return runDory();
}