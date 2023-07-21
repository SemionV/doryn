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
    auto windowSystem = std::make_shared<doryWindows::WindowSystemParallel>();
    auto windowSystemListener = std::make_shared<dory::DeviceListener>();
    windowSystem->attachListener(windowSystemListener);

    auto consoleSystem = std::make_shared<doryWindows::ConsoleSystem>();
    auto consoleSystemListener = std::make_shared<dory::DeviceListener>();
    consoleSystem->attachListener(consoleSystemListener);

    dory::InputController inputController(inputMessagePool);
    engine.addController(&inputController);

    inputController.addDevice(consoleSystem);
    inputController.addDeviceListener(consoleSystemListener);

    inputController.addDevice(windowSystem);
    inputController.addDeviceListener(windowSystemListener);

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

class TestClass
{
    private:
        int testMember = 4;

    public:
        void eventHandler(int a, int b)
        {
            std::cout << "event 3: " << a + b + testMember<< std::endl;
        }
};

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

    dory::Event<int, int> event;
    auto handler1 = event.attachHandler([](int a, int b)
    {
        std::cout << "event 1: " << a + b << std::endl;
    });

    auto handler2 = event.attachHandler([](int a, int b)
    {
        std::cout << "event 2: " << a + b << std::endl;
    });

    TestClass testObject;
    std::function<void(int, int)> f = std::bind(&TestClass::eventHandler, &testObject, std::placeholders::_1, std::placeholders::_2);
    auto handler3 = event.attachHandler(f);

    event(3, 4);

    event.detachHandler(handler2);

    event(3, 4);

    event.detachHandler(handler1);

    event(3, 4);

    return runDory();
}