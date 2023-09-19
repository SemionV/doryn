#include "dependencies.h"

#include <windows.h>

#include <thread>
#include <iostream>
#include <functional>
#include <mutex>

using EntityId = dory::domain::entity::IdType;

void configureWin32Window(dory::DataContext& context, std::shared_ptr<dory::Engine> engine, std::shared_ptr<dory::NumberIdFactory<EntityId>> idFactory)
{
    auto windowsThread = std::make_shared<dory::IndividualProcessThread>();
    auto windowRespositoryWin32 = std::make_shared<dory::EntityRepository<dory::win32::Window>>();
    auto windowAccessorWin32 = std::make_shared<dory::RepositoryReader<dory::win32::Window>>(windowRespositoryWin32);
    auto messageBufferWin32 = std::make_shared<dory::win32::MessageBuffer>();
    auto win32WindowEventHub = std::make_shared<dory::WindowEventHubDispatcher>();
    auto windowControllerWin32 = std::make_shared<dory::win32::WindowControllerParallel>(windowsThread, win32WindowEventHub, messageBufferWin32, windowAccessorWin32);
    windowControllerWin32->initialize(context);
    engine->addController(windowControllerWin32);

    windowsThread->run();

    dory::win32::WindowParameters win32WindowParameters;
    auto hWnd = dory::win32::WindowFactory::createWindow(win32WindowParameters, messageBufferWin32.get(), windowsThread);
    auto win32Window = windowRespositoryWin32->store(dory::win32::Window(idFactory->generate(), hWnd));

    win32WindowEventHub->onCloseWindow() += [&windowRespositoryWin32, &windowAccessorWin32, &windowsThread](dory::DataContext& context, dory::CloseWindowEventData& eventData)
        {
            auto windowId = eventData.windowId;
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
}

int runDory()
{
    auto configuration = std::make_shared<dory::FileSystemBasedConfiguration>("configuration");
    
    auto idFactory = std::make_shared<dory::NumberIdFactory<EntityId>>();
    auto windowRespository = std::make_shared<dory::EntityRepository<dory::openGL::GlfwWindow>>();
    auto cameraRepository = std::make_shared<dory::EntityRepository<dory::domain::entity::Camera>>();
    auto viewRepository = std::make_shared<dory::EntityRepository<dory::domain::entity::View>>();

    auto windowAccessor = std::make_shared<dory::RepositoryReader<dory::openGL::GlfwWindow>>(windowRespository);
    auto cameraAccessor = std::make_shared<dory::RepositoryReader<dory::domain::entity::Camera>>(cameraRepository);
    auto viewAccessor = std::make_shared<dory::RepositoryReader<dory::domain::entity::View>>(viewRepository);

    dory::DataContext context;
    auto engine = std::make_shared<dory::Engine>(context);

    auto consoleEventHub = std::make_shared<dory::SystemConsoleEventHubDispatcher>();
    auto consoleController = std::make_shared<dory::win32::ConsoleController>(consoleEventHub);
    consoleController->initialize(context);
    engine->addController(consoleController);

    configureWin32Window(context, engine, idFactory);

    auto glfwWindowEventHub = std::make_shared<dory::WindowEventHubDispatcher>();
    auto windowController = std::make_shared<dory::openGL::GlfwWindowController>(windowAccessor, glfwWindowEventHub);
    windowController->initialize(context);
    engine->addController(windowController);

    dory::openGL::GlfwWindowParameters glfwWindowParameters;
    auto glfwWindowHandler = dory::openGL::GlfwWindowFactory::createWindow(glfwWindowParameters);
    auto window = windowRespository->store(dory::openGL::GlfwWindow(idFactory->generate(), glfwWindowHandler));

    auto camera = cameraRepository->store(dory::domain::entity::Camera(idFactory->generate()));
    dory::domain::entity::Viewport viewport(0, 0, 0, 0);

    auto view = viewRepository->store(dory::domain::entity::View(idFactory->generate(), window.id, camera.id, viewport));

    auto viewController = std::make_shared<dory::openGL::ViewControllerOpenGL>(view.id, viewAccessor, configuration, windowAccessor);
    engine->addController(viewController);
    viewController->initialize(context);

    glfwWindowEventHub->onCloseWindow() += [&windowRespository, &windowAccessor, &viewAccessor, &viewRepository](dory::DataContext& context, dory::CloseWindowEventData& eventData)
        {
            auto windowId = eventData.windowId;
            auto window = windowAccessor->get(windowId);

            if(window)
            {
                auto windowHandler = window->handler;

                dory::openGL::GlfwWindowFactory::closeWindow(windowHandler);

                context.isStop = true;
                std::cout << "Close window(id " << windowId << ")" << std::endl;

                windowRespository->remove(window);

                std::list<dory::domain::entity::View*> attachedViews;
                viewAccessor->list(windowId, [](dory::domain::entity::View* view, EntityId windowId) 
                    {
                        return view->windowId == windowId;
                    }, attachedViews);
                auto i = attachedViews.begin();
                auto end = attachedViews.end();
                while(i != end)
                {
                    auto view = (*i);
                    std::cout << "remove view(id " << view->id << ")" << std::endl;
                    viewRepository->remove(view);
                    ++i;
                }
            }
        };

    test::TestLogic logic(consoleEventHub, glfwWindowEventHub);

    engine->initialize(context);

    std::cout << "dory:native demo application" << std::endl;
    std::cout << "Press any key to process to render frame OR ESC to exit\r" << std::endl;

    auto frameService = std::make_shared<dory::BasicFrameService>();
    frameService->startLoop(engine);

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