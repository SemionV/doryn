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
    
    auto glfwWindowEventHub = std::make_shared<dory::openGL::GlfwWindowEventHubDispatcher>();
    auto glfwWindowSystem = std::make_shared<dory::openGL::GlfwWindowSystem>(glfwWindowEventHub);
    glfwWindowSystem->connect();

    auto consoleEventHub = std::make_shared<dory::SystemConsoleEventHubDispatcher>();
    auto consoleSystem = std::make_shared<doryWindows::ConsoleSystem>(consoleEventHub);
    consoleSystem->connect();

    auto inputController = std::make_shared<dory::InputController>();
    engine.addController(inputController);
    inputController->addDevice(consoleSystem);
    inputController->addDevice(glfwWindowSystem);
    inputController->initialize(context);

    const dory::openGL::GlfwWindowParameters glfwWindowParameters;
    auto glfwWindow = glfwWindowSystem->createWindow(glfwWindowParameters);
    auto viewport = std::make_shared<dory::Viewport>(0, 0, 0, 0);
    auto camera = std::make_shared<dory::Camera>();
    auto view = std::make_shared<dory::View<dory::openGL::GlfwWindow>>(glfwWindow, viewport, camera);

    test::TestLogic logic(consoleEventHub, glfwWindowEventHub);

    auto windowRespository = std::make_shared<dory::EntityRepository<dory::openGL::GlfwWindow>>();
    auto cameraRepository = std::make_shared<dory::EntityRepository<dory::CameraNi>>();
    auto viewRepository = std::make_shared<dory::ViewEntityRepository>();

    auto glfwWindowHandler = dory::openGL::GlfwWindowFactory::createWindow(glfwWindowParameters);
    auto windowN = windowRespository->store(glfwWindowHandler);

    auto cameraN = cameraRepository->store();
    dory::ViewportNi viewportN(0, 0, 0, 0);

    auto viewN = viewRepository->store(windowN.id, cameraN.id, viewportN);
    auto viewN2 = viewRepository->store(windowN.id, cameraN.id, viewportN);

    auto windowController = std::make_shared<dory::openGL::GlfwWindowController>(windowRespository, glfwWindowEventHub);
    engine.addController(windowController);

    auto viewController = std::make_shared<dory::openGL::ViewControllerOpenGL>(configuration, view);
    engine.addController(viewController);
    viewController->initialize(context);

    glfwWindowEventHub->onCloseWindow() += [&glfwWindowSystem, &viewController, &engine](dory::DataContext& context, dory::openGL::CloseWindowEventData& eventData)
        {
            if(!eventData.pWindow)
            {
                context.isStop = true;
                std::cout << "Close main window" << std::endl;
                engine.removeController(viewController);
                glfwWindowSystem->closeWindow(eventData.window);
            }
        };

    glfwWindowEventHub->onCloseWindow() += [&windowRespository, &viewRepository](dory::DataContext& context, dory::openGL::CloseWindowEventData& eventData)
        {
            if(eventData.pWindow)
            {
                int windowId = eventData.pWindow->id;

                //context.isStop = true;
                std::cout << "Close window(id " << windowId << ")" << std::endl;

                windowRespository->remove(windowId);

                std::list<dory::ViewNi*> attachedViews;
                viewRepository->getList(windowId, attachedViews);
                auto end = attachedViews.end();
                auto i = attachedViews.begin();
                while(i != end)
                {
                    std::cout << "remove view(id " << (*i)->id << ")" << std::endl;
                    viewRepository->remove((*i)->id);
                    ++i;
                }

                dory::openGL::GlfwWindowFactory::closeWindow(eventData.pWindow);
            }
        };

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