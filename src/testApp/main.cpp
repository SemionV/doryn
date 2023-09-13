#include "dependencies.h"

#include <windows.h>

#include <thread>
#include <iostream>
#include <functional>
#include <mutex>

int runDory()
{
    auto configuration = std::make_shared<dory::FileSystemBasedConfiguration>("configuration");

    dory::DataContext context;
    dory::Engine engine(context);
    
    auto glfwWindowEventHub = std::make_shared<doryOpenGL::GlfwWindowEventHubDispatcher>();
    auto glfwWindowSystem = std::make_shared<doryOpenGL::GlfwWindowSystem>(glfwWindowEventHub);
    glfwWindowSystem->connect();

    glfwWindowEventHub->onCloseWindow() += [&glfwWindowSystem](dory::DataContext& dataContext, doryOpenGL::CloseWindowEventData& eventData)
    {
        dataContext.isStop = true;
        std::cout << "Close app because the main window was closed." << std::endl;
        glfwWindowSystem->closeWindow(eventData.window);
    };

    auto consoleEventHub = std::make_shared<dory::SystemConsoleEventHubDispatcher>();
    auto consoleSystem = std::make_shared<doryWindows::ConsoleSystem>(consoleEventHub);
    consoleSystem->connect();

    auto inputController = std::make_shared<dory::InputController>();
    engine.addController(inputController);
    inputController->addDevice(consoleSystem);
    inputController->addDevice(glfwWindowSystem);
    inputController->initialize(context);

    const doryOpenGL::GlfwWindowParameters glfwWindowParameters;
    auto glfwWindow = glfwWindowSystem->createWindow(glfwWindowParameters);
    auto viewport = std::make_shared<dory::Viewport>(0, 0, 0, 0);
    auto camera = std::make_shared<dory::Camera>();
    auto view = std::make_shared<dory::View<doryOpenGL::GlfwWindow>>(glfwWindow, viewport, camera);

    auto viewController = std::make_shared<doryOpenGL::ViewControllerOpenGL>(configuration, view);
    engine.addController(viewController);
    viewController->initialize(context);

    test::TestLogic logic(consoleEventHub, glfwWindowEventHub);

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