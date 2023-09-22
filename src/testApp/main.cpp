#include "dependencies.h"

#include <windows.h>

#include <thread>
#include <iostream>
#include <functional>
#include <mutex>

using EntityId = dory::domain::entity::IdType;



int runDory()
{
    auto configuration = std::make_shared<dory::configuration::FileSystemBasedConfiguration>("configuration");
    
    auto idFactory = std::make_shared<dory::NumberIdFactory<EntityId>>();
    auto windowRespository = std::make_shared<dory::EntityRepository<dory::openGL::GlfwWindow>>();
    auto cameraRepository = std::make_shared<dory::EntityRepository<dory::domain::entity::Camera>>();
    auto viewRepository = std::make_shared<dory::EntityRepository<dory::domain::entity::View>>();

    auto windowReader = std::make_shared<dory::RepositoryReader<dory::openGL::GlfwWindow>>(windowRespository);
    auto cameraReader = std::make_shared<dory::RepositoryReader<dory::domain::entity::Camera>>(cameraRepository);
    auto viewReader = std::make_shared<dory::RepositoryReader<dory::domain::entity::View>>(viewRepository);

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

    auto consoleEventHub = std::make_shared<dory::events::SystemConsoleEventHubDispatcher>();
    auto consoleController = std::make_shared<dory::win32::ConsoleController>(consoleEventHub);
    auto consoleControllerNode = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(idFactory->generate(), 
        consoleController, 0, inpoutGroupNode.id));
    consoleController->initialize(consoleControllerNode.id, context);

    auto glfwWindowEventHub = std::make_shared<dory::events::WindowEventHubDispatcher>();
    auto windowController = std::make_shared<dory::openGL::GlfwWindowController>(windowReader, glfwWindowEventHub);
    auto windowControllerNode = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(idFactory->generate(), 
        windowController, 0, inpoutGroupNode.id));
    windowController->initialize(windowControllerNode.id, context);

    consoleEventHub->onKeyPressed() += [] (dory::DataContext& context, dory::events::KeyPressedEventData& keyPressedEventData)
    {
        if(keyPressedEventData.keyPressed == 27)
        {
            context.isStop = true;
            std::cout << std::this_thread::get_id() << ": ESC" << std::endl;
        }
        else if(keyPressedEventData.keyPressed != 0)
        {
            std::cout << std::this_thread::get_id() << ": key pressed: " << keyPressedEventData.keyPressed << std::endl;
        }
    };

    glfwWindowEventHub->onCloseWindow() += [&](dory::DataContext& context, dory::events::CloseWindowEventData& eventData)
    {
        auto windowId = eventData.windowId;
        auto window = windowReader->get(windowId);

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

    engineEventHub->onInitializeEngine() += [&](dory::DataContext& dataContext, const dory::events::InitializeEngineEventData& eventData)
    {
        dory::openGL::GlfwWindowParameters glfwWindowParameters;
        auto glfwWindowHandler = dory::openGL::GlfwWindowFactory::createWindow(glfwWindowParameters);
        auto window = windowRespository->store(dory::openGL::GlfwWindow(idFactory->generate(), glfwWindowHandler));

        auto camera = cameraRepository->store(dory::domain::entity::Camera(idFactory->generate()));
        dory::domain::entity::Viewport viewport(0, 0, 0, 0);

        auto viewController = std::make_shared<dory::openGL::ViewControllerOpenGL>(viewReader, configuration, windowReader);
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