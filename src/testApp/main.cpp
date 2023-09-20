#include "dependencies.h"

#include <windows.h>

#include <thread>
#include <iostream>
#include <functional>
#include <mutex>

using EntityId = dory::domain::entity::IdType;

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

    auto pipelineNodeRepository = std::make_shared<dory::EntityRepository<dory::domain::entity::PipelineNode>>();
    auto pipelineNodeReader = std::make_shared<dory::RepositoryReader<dory::domain::entity::PipelineNode>>(pipelineNodeRepository);
    auto pipelineService = std::make_shared<dory::domain::services::PipelineService>(pipelineNodeReader);

    auto inpoutGroupNode = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(idFactory->generate(), 
        nullptr, 0, dory::domain::entity::nullId, "input group"));
    auto outputGroupNode = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(idFactory->generate(), 
        nullptr, 1, dory::domain::entity::nullId, "output group"));

    dory::DataContext context;
    auto engineEventHub = std::make_shared<dory::events::EngineEventHubDispatcher>();
    auto engine = std::make_shared<dory::domain::Engine>(pipelineService, engineEventHub);

    auto consoleEventHub = std::make_shared<dory::SystemConsoleEventHubDispatcher>();
    auto consoleController = std::make_shared<dory::win32::ConsoleController>(consoleEventHub);
    auto consoleControllerNode = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(idFactory->generate(), 
        consoleController, 0, inpoutGroupNode.id));
    consoleController->initialize(consoleControllerNode.id, context);

    //win32
    auto windowsThread = std::make_shared<dory::IndividualProcessThread>();
    auto windowRespositoryWin32 = std::make_shared<dory::EntityRepository<dory::win32::Window>>();
    auto windowAccessorWin32 = std::make_shared<dory::RepositoryReader<dory::win32::Window>>(windowRespositoryWin32);
    auto messageBufferWin32 = std::make_shared<dory::win32::MessageBuffer>();
    auto win32WindowEventHub = std::make_shared<dory::WindowEventHubDispatcher>();
    auto windowControllerWin32 = std::make_shared<dory::win32::WindowControllerParallel>(windowsThread, win32WindowEventHub, messageBufferWin32, windowAccessorWin32);
    auto windowControllerWin32Node = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(idFactory->generate(), 
        windowControllerWin32, 0, inpoutGroupNode.id));
    windowControllerWin32->initialize(windowControllerWin32Node.id, context);

    windowsThread->run();

    dory::win32::WindowParameters win32WindowParameters;
    auto hWnd = dory::win32::WindowFactory::createWindow(win32WindowParameters, messageBufferWin32.get(), windowsThread);
    auto win32Window = windowRespositoryWin32->store(dory::win32::Window(idFactory->generate(), hWnd));

    win32WindowEventHub->onCloseWindow() += [&](dory::DataContext& context, dory::CloseWindowEventData& eventData)
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
    //~win32

    auto glfwWindowEventHub = std::make_shared<dory::WindowEventHubDispatcher>();
    auto windowController = std::make_shared<dory::openGL::GlfwWindowController>(windowAccessor, glfwWindowEventHub);
    auto windowControllerNode = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(idFactory->generate(), 
        windowController, 0, inpoutGroupNode.id));
    windowController->initialize(windowControllerNode.id, context);

    glfwWindowEventHub->onCloseWindow() += [&](dory::DataContext& context, dory::CloseWindowEventData& eventData)
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
            viewRepository->remove([&windowId](dory::domain::entity::View& view)
            {
                return view.windowId == windowId;
            });
        }
    };

    test::TestLogic logic(consoleEventHub, glfwWindowEventHub);

    engineEventHub->onInitializeEngine() += [&](dory::DataContext& dataContext, const dory::events::InitializeEngineEventData& eventData)
    {
        dory::openGL::GlfwWindowParameters glfwWindowParameters;
        auto glfwWindowHandler = dory::openGL::GlfwWindowFactory::createWindow(glfwWindowParameters);
        auto window = windowRespository->store(dory::openGL::GlfwWindow(idFactory->generate(), glfwWindowHandler));

        auto camera = cameraRepository->store(dory::domain::entity::Camera(idFactory->generate()));
        dory::domain::entity::Viewport viewport(0, 0, 0, 0);

        auto viewController = std::make_shared<dory::openGL::ViewControllerOpenGL>(viewAccessor, configuration, windowAccessor);
        auto viewControllerNode = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(idFactory->generate(), 
            viewController, 0, outputGroupNode.id));

        auto view = viewRepository->store(dory::domain::entity::View(idFactory->generate(), window.id, viewControllerNode.id, camera.id, viewport));

        viewController->initialize(viewControllerNode.id, context);
    };

    engine->initialize(context);

    std::cout << "dory:native demo application" << std::endl;
    std::cout << "Press any key to process to render frame OR ESC to exit\r" << std::endl;

    auto frameService = std::make_shared<dory::BasicFrameService>();
    frameService->startLoop(engine, context);

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