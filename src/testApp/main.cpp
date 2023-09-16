#include "dependencies.h"

#include <windows.h>

#include <thread>
#include <iostream>
#include <functional>
#include <mutex>

int runDory()
{
    auto configuration = std::make_shared<dory::FileSystemBasedConfiguration>("configuration");
    
    auto windowRespository = std::make_shared<dory::EntityRepository<dory::openGL::GlfwWindow, int>>();
    auto cameraRepository = std::make_shared<dory::EntityRepository<dory::Camera, int>>();
    auto viewRepository = std::make_shared<dory::EntityRepository<dory::View, int>>();

    auto windowAccessor = std::make_shared<dory::EntityAccessor<dory::openGL::GlfwWindow>>(windowRespository);
    auto cameraAccessor = std::make_shared<dory::EntityAccessor<dory::Camera>>(cameraRepository);
    auto viewAccessor = std::make_shared<dory::EntityAccessor<dory::View>>(viewRepository);

    dory::DataContext context;
    dory::Engine engine(context);

    auto consoleEventHub = std::make_shared<dory::SystemConsoleEventHubDispatcher>();
    auto consoleController = std::make_shared<dory::win32::ConsoleController>(consoleEventHub);
    consoleController->initialize(context);
    engine.addController(consoleController);

    auto glfwWindowEventHub = std::make_shared<dory::WindowEventHubDispatcher>();
    auto windowController = std::make_shared<dory::openGL::GlfwWindowController>(windowAccessor, glfwWindowEventHub);
    windowController->initialize(context);
    engine.addController(windowController);

    //win32
    auto windowsThread = std::make_shared<dory::IndividualProcessThread>();
    auto windowRespositoryWin32 = std::make_shared<dory::EntityRepository<dory::win32::Window, int>>();
    auto windowAccessorWin32 = std::make_shared<dory::EntityAccessor<dory::win32::Window>>(windowRespositoryWin32);
    auto messageBufferWin32 = std::make_shared<dory::win32::MessageBuffer>();
    auto win32WindowEventHub = std::make_shared<dory::WindowEventHubDispatcher>();
    auto windowControllerWin32 = std::make_shared<dory::win32::WindowControllerParallel>(windowsThread, win32WindowEventHub, messageBufferWin32, windowAccessorWin32);
    windowControllerWin32->initialize(context);
    engine.addController(windowControllerWin32);

    windowsThread->run();

    dory::win32::WindowParameters win32WindowParameters;
    auto hWnd = dory::win32::WindowFactory::createWindow(win32WindowParameters, messageBufferWin32.get(), windowsThread);
    auto win32Window = windowRespositoryWin32->store(dory::win32::Window(hWnd));

    win32WindowEventHub->onCloseWindow() += [&windowRespositoryWin32, &windowAccessorWin32, &windowsThread](dory::DataContext& context, dory::CloseWindowEventData& eventData)
        {
            int windowId = eventData.windowId;
            auto window = windowAccessorWin32->get(windowId);

            if(window)
            {
                auto hWnd = window->hWnd;

                context.isStop = true;
                std::cout << "Close window(id " << windowId << ")" << std::endl;

                dory::win32::WindowFactory::closeWindow(hWnd, windowsThread);
                windowRespositoryWin32->remove(window);
            }
        };

    //~win32

    dory::openGL::GlfwWindowParameters glfwWindowParameters;
    auto glfwWindowHandler = dory::openGL::GlfwWindowFactory::createWindow(glfwWindowParameters);
    auto window = windowRespository->store(dory::openGL::GlfwWindow(glfwWindowHandler));

    auto camera = cameraRepository->store(dory::Camera());
    dory::Viewport viewport(0, 0, 0, 0);

    auto view = viewRepository->store(dory::View(window.id, camera.id, viewport));

    auto viewController = std::make_shared<dory::openGL::ViewControllerOpenGL>(view.id, viewAccessor, configuration, windowAccessor);
    engine.addController(viewController);
    viewController->initialize(context);

    glfwWindowEventHub->onCloseWindow() += [&windowRespository, &windowAccessor, &viewAccessor, &viewRepository](dory::DataContext& context, dory::CloseWindowEventData& eventData)
        {
            int windowId = eventData.windowId;
            auto window = windowAccessor->get(windowId);

            if(window)
            {
                auto windowHandler = window->handler;

                dory::openGL::GlfwWindowFactory::closeWindow(windowHandler);

                context.isStop = true;
                std::cout << "Close window(id " << windowId << ")" << std::endl;

                windowRespository->remove(window);

                std::list<dory::View*> attachedViews;
                viewAccessor->list(windowId, [](dory::View* view, int windowId) 
                    {
                        return view->windowId == windowId;
                    }, attachedViews);
                auto end = attachedViews.end();
                auto i = attachedViews.begin();
                while(i != end)
                {
                    std::cout << "remove view(id " << (*i)->id << ")" << std::endl;
                    viewRepository->remove(*i);
                    ++i;
                }
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