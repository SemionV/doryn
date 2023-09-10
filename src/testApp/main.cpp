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
    windowSystem->connect();

    auto glfwWindowSystem = std::make_shared<doryOpenGL::GlfwWindowSystem>(windowEventHub);
    glfwWindowSystem->connect();

    auto consoleEventHub = std::make_shared<dory::SystemConsoleEventHubDispatcher>();
    auto consoleSystem = std::make_shared<doryWindows::ConsoleSystem>(consoleEventHub);
    consoleSystem->connect();

    auto inputController = std::make_shared<dory::InputController>(consoleEventHub, windowEventHub);
    engine.addController(inputController);
    inputController->addDevice(consoleSystem);
    inputController->addDevice(windowSystem);
    inputController->addDevice(glfwWindowSystem);
    inputController->initialize(context);

    const doryWindows::WindowParameters windowParameters;
    auto window = windowSystem->createWindow(windowParameters);

    const doryOpenGL::GlfwWindowParameters glfwWindowParameters;
    auto glfwWindow = glfwWindowSystem->createWindow(glfwWindowParameters);
    auto viewport = std::make_shared<dory::Viewport>(0, 0, 0, 0);
    auto camera = std::make_shared<dory::Camera>();
    auto view = std::make_shared<dory::View>(glfwWindow, viewport, camera);

    auto viewController = std::make_shared<doryOpenGL::ViewControllerOpenGL>(view);
    engine.addController(viewController);
    viewController->initialize(context);

    test::TestLogic logic(consoleEventHub, windowEventHub);

    engine.initialize(context);

    std::cout << "dory:native demo application" << std::endl;
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