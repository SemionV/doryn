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

    auto consoleEventHub = std::make_shared<dory::SystemConsoleEventHubDispatcher>();
    auto consoleController = std::make_shared<dory::win32::Win32ConsoleController>(consoleEventHub);
    consoleController->initialize(context);
    engine.addController(consoleController);

    auto glfwWindowEventHub = std::make_shared<dory::WindowEventHubDispatcher>();
    auto windowController = std::make_shared<dory::openGL::GlfwWindowController>(windowRespository, glfwWindowEventHub);
    windowController->initialize(context);
    engine.addController(windowController);

    //win32
    auto windowsThread = std::make_shared<dory::IndividualProcessThread>();
    auto windowRespositoryWin32 = std::make_shared<dory::win32::Win32WindowRespository>();
    auto messageBufferWin32 = std::make_shared<dory::win32::Win32MessageBuffer>();
    auto win32WindowEventHub = std::make_shared<dory::WindowEventHubDispatcher>();
    auto windowControllerWin32 = std::make_shared<dory::win32::Win32WindowControllerParallel>(windowsThread, win32WindowEventHub, messageBufferWin32, windowRespositoryWin32);
    windowControllerWin32->initialize(context);
    engine.addController(windowControllerWin32);

    windowsThread->run();

    dory::win32::Win32WindowParameters win32WindowParameters;
    auto hWnd = dory::win32::Win32WindowFactory::createWindow(win32WindowParameters, messageBufferWin32.get(), windowsThread);
    auto win32Window = windowRespositoryWin32->store(hWnd);

    win32WindowEventHub->onCloseWindow() += [&windowRespositoryWin32, &windowsThread](dory::DataContext& context, dory::CloseWindowEventData& eventData)
        {
            int windowId = eventData.windowId;
            auto window = windowRespositoryWin32->get(windowId);

            if(window.has_value())
            {
                auto hWnd = window.value().hWnd;

                context.isStop = true;
                std::cout << "Close window(id " << windowId << ")" << std::endl;

                windowRespositoryWin32->remove(windowId);
                dory::win32::Win32WindowFactory::closeWindow(hWnd, windowsThread);
            }
        };

    //~win32

    dory::openGL::GlfwWindowParameters glfwWindowParameters;
    auto glfwWindowHandler = dory::openGL::GlfwWindowFactory::createWindow(glfwWindowParameters);
    auto window = windowRespository->store(glfwWindowHandler);

    auto camera = cameraRepository->store();
    dory::Viewport viewport(0, 0, 0, 0);

    auto view = viewRepository->store(window.id, camera.id, viewport);

    auto viewController = std::make_shared<dory::openGL::ViewControllerOpenGL>(view.id, viewRepository, configuration, windowRespository);
    engine.addController(viewController);
    viewController->initialize(context);

    glfwWindowEventHub->onCloseWindow() += [&windowRespository, &viewRepository](dory::DataContext& context, dory::CloseWindowEventData& eventData)
        {
            int windowId = eventData.windowId;
            auto window = windowRespository->get(windowId);

            if(window.has_value())
            {
                auto windowHandler = window.value().handler;

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

                dory::openGL::GlfwWindowFactory::closeWindow(windowHandler);
            }
        };

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