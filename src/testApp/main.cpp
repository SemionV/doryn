#include "dependencies.h"

#include <windows.h>

#include <thread>
#include <iostream>
#include <functional>
#include <mutex>

int runDory()
{
    auto configuration = std::make_shared<dory::FileSystemBasedConfiguration>("configuration");
    auto windowRespository = std::make_shared<dory::EntityRepository<dory::openGL::GlfwWindow>>();
    auto cameraRepository = std::make_shared<dory::EntityRepository<dory::Camera>>();
    auto viewRepository = std::make_shared<dory::ViewEntityRepository>();

    dory::DataContext context;
    dory::Engine engine(context);

    /*auto consoleEventHub = std::make_shared<dory::SystemConsoleEventHubDispatcher>();
    auto consoleSystem = std::make_shared<doryWindows::ConsoleSystem>(consoleEventHub);
    consoleSystem->connect();*/

    /*auto inputController = std::make_shared<dory::InputController>();
    engine.addController(inputController);
    inputController->addDevice(consoleSystem);
    inputController->initialize(context);*/

    auto glfwWindowEventHub = std::make_shared<dory::openGL::GlfwWindowEventHubDispatcher>();
    auto windowController = std::make_shared<dory::openGL::GlfwWindowController>(windowRespository, glfwWindowEventHub);
    windowController->initialize(context);
    engine.addController(windowController);

    dory::openGL::GlfwWindowParameters glfwWindowParameters;
    auto glfwWindowHandler = dory::openGL::GlfwWindowFactory::createWindow(glfwWindowParameters);
    auto window = windowRespository->store(glfwWindowHandler);

    auto camera = cameraRepository->store();
    dory::Viewport viewport(0, 0, 0, 0);

    auto view = viewRepository->store(window.id, camera.id, viewport);

    auto viewController = std::make_shared<dory::openGL::ViewControllerOpenGL>(view.id, viewRepository, configuration, windowRespository);
    engine.addController(viewController);
    viewController->initialize(context);

    glfwWindowEventHub->onCloseWindow() += [&windowRespository, &viewRepository](dory::DataContext& context, dory::openGL::CloseWindowEventData& eventData)
        {
            if(eventData.pWindow)
            {
                int windowId = eventData.pWindow->id;

                context.isStop = true;
                std::cout << "Close window(id " << windowId << ")" << std::endl;

                windowRespository->remove(windowId);

                std::list<dory::View*> attachedViews;
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

    //test::TestLogic logic(consoleEventHub, glfwWindowEventHub);

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