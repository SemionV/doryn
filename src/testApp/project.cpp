#include "dependencies.h"
#include "project.h"

namespace testApp
{
    std::shared_ptr<Engine> Project::getEngine()
    {
        return std::make_shared<dory::domain::Engine>(pipelineService, engineEventHub);
    }

    std::shared_ptr<services::IFrameService> Project::getFrameService()
    {
        return std::make_shared<dory::domain::services::BasicFrameService>();
    }

    std::shared_ptr<ProjectDataContext> Project::getDataContext()
    {
        return std::make_shared<ProjectDataContext>();
    }

    void Project::configureProject()
    {
        configuration = std::make_shared<configuration::FileSystemBasedConfiguration>("configuration");

        windowIdFactory = std::make_shared<NumberIdFactory<EntityId>>();
        windowRespository = std::make_shared<EntityRepository<openGL::GlfwWindow>>();
        windowRepositoryReader = std::make_shared<RepositoryReader<dory::openGL::GlfwWindow>>(windowRespository);

        cameraIdFactory = std::make_shared<NumberIdFactory<EntityId>>();
        cameraRepository = std::make_shared<EntityRepository<entity::Camera>>();
        cameraRepositoryReader = std::make_shared<RepositoryReader<entity::Camera>>(cameraRepository);

        viewIdFactory = std::make_shared<NumberIdFactory<EntityId>>();
        viewRepository = std::make_shared<EntityRepository<entity::View>>();
        viewRepositoryReader = std::make_shared<RepositoryReader<entity::View>>(viewRepository);

        pipelineNodeIdFactory = std::make_shared<NumberIdFactory<EntityId>>();
        pipelineNodeRepository = std::make_shared<EntityRepository<entity::PipelineNode>>();
        pipelineNodeRepositoryReader = std::make_shared<RepositoryReader<entity::PipelineNode>>(pipelineNodeRepository);

        pipelineService = std::make_shared<services::PipelineService>(pipelineNodeRepositoryReader);

        engineEventHub = std::make_shared<events::EngineEventHubDispatcher>();
        consoleEventHub = std::make_shared<events::SystemConsoleEventHubDispatcher>();
        glfwWindowEventHub = std::make_shared<events::WindowEventHubDispatcher>();
    }

    void Project::configurePipeline(ProjectDataContext& context)
    {
        auto inputGroupNode = pipelineNodeRepository->store(entity::PipelineNode(pipelineNodeIdFactory->generate(), nullptr, 0, entity::nullId, "input group"));
        auto outputGroupNode = pipelineNodeRepository->store(entity::PipelineNode(pipelineNodeIdFactory->generate(), nullptr, 1, entity::nullId, "output group"));
        context.inputGroupNodeId = inputGroupNode.id;
        context.outputGroupNodeId = outputGroupNode.id;

        auto consoleController = std::make_shared<dory::win32::ConsoleController>(consoleEventHub);
        auto consoleControllerNode = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(pipelineNodeIdFactory->generate(), consoleController, 0, inputGroupNode.id));
        consoleController->initialize(consoleControllerNode.id, context);

        auto windowController = std::make_shared<dory::openGL::GlfwWindowController>(windowRepositoryReader, glfwWindowEventHub);
        auto windowControllerNode = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(pipelineNodeIdFactory->generate(), windowController, 0, inputGroupNode.id));
        windowController->initialize(windowControllerNode.id, context);
    }

    void Project::attachEventHandlers()
    {
        engineEventHub->onInitializeEngine() += std::bind(&Project::onInitializeEngine, this, std::placeholders::_1, std::placeholders::_2);
        consoleEventHub->onKeyPressed() += std::bind(&Project::onConsoleKeyPressed, this, std::placeholders::_1, std::placeholders::_2);
        glfwWindowEventHub->onCloseWindow() += std::bind(&Project::onCloseWindow, this, std::placeholders::_1, std::placeholders::_2);
    }

    void Project::onInitializeEngine(DataContext& context, const events::InitializeEngineEventData& eventData)
    {
        auto projectContext = static_cast<ProjectDataContext&>(context);

        dory::openGL::GlfwWindowParameters glfwWindowParameters;
        auto glfwWindowHandler = dory::openGL::GlfwWindowFactory::createWindow(glfwWindowParameters);
        auto window = windowRespository->store(dory::openGL::GlfwWindow(windowIdFactory->generate(), glfwWindowHandler));

        auto camera = cameraRepository->store(dory::domain::entity::Camera(cameraIdFactory->generate()));
        dory::domain::entity::Viewport viewport(0, 0, 0, 0);

        auto viewController = std::make_shared<dory::openGL::ViewControllerOpenGL>(viewRepositoryReader, configuration, windowRepositoryReader);
        auto viewControllerNode = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(pipelineNodeIdFactory->generate(), 
            viewController, 0, projectContext.outputGroupNodeId));

        auto view = viewRepository->store(dory::domain::entity::View(viewIdFactory->generate(), window.id, viewControllerNode.id, camera.id, viewport));

        viewController->initialize(viewControllerNode.id, context);
    }

    void Project::onConsoleKeyPressed(DataContext& context, events::KeyPressedEventData& eventData)
    {
        if(eventData.keyPressed == 27)
        {
            context.isStop = true;
            std::cout << std::this_thread::get_id() << ": ESC" << std::endl;
        }
        else if(eventData.keyPressed != 0)
        {
            std::cout << std::this_thread::get_id() << ": key pressed: " << eventData.keyPressed << std::endl;
        }
    }

    void Project::onCloseWindow(DataContext& context, events::CloseWindowEventData& eventData)
    {
        auto windowId = eventData.windowId;
        auto window = windowRepositoryReader->get(windowId);

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
    }
}