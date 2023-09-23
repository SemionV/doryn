#pragma once

namespace testApp
{
    using EntityId = dory::domain::entity::IdType;
    using namespace dory;
    using namespace domain;

    template<class TDataContext>
    class Project: public dory::Project<TDataContext>
    {
        private:
            std::shared_ptr<configuration::IConfiguration> configuration;

            std::shared_ptr<IIdFactory<EntityId>> windowIdFactory;
            std::shared_ptr<EntityRepository<openGL::GlfwWindow>> windowRespository;
            std::shared_ptr<RepositoryReader<openGL::GlfwWindow>> windowRepositoryReader;

            std::shared_ptr<IIdFactory<EntityId>> cameraIdFactory;
            std::shared_ptr<EntityRepository<entity::Camera>> cameraRepository;
            std::shared_ptr<RepositoryReader<entity::Camera>> cameraRepositoryReader;

            std::shared_ptr<IIdFactory<EntityId>> viewIdFactory;
            std::shared_ptr<EntityRepository<entity::View>> viewRepository;
            std::shared_ptr<RepositoryReader<entity::View>> viewRepositoryReader;
            
            std::shared_ptr<IIdFactory<EntityId>> pipelineNodeIdFactory;
            std::shared_ptr<EntityRepository<entity::PipelineNode>> pipelineNodeRepository;
            std::shared_ptr<RepositoryReader<entity::PipelineNode>> pipelineNodeRepositoryReader;

            std::shared_ptr<services::PipelineService> pipelineService;

            std::shared_ptr<events::EngineEventHubDispatcher<TDataContext>> engineEventHub;
            std::shared_ptr<events::SystemConsoleEventHubDispatcher<TDataContext>> consoleEventHub;
            std::shared_ptr<events::WindowEventHubDispatcher<TDataContext>> glfwWindowEventHub;

        protected:
            std::shared_ptr<Engine<TDataContext>> getEngine() override
            {
                return std::make_shared<dory::domain::Engine<TDataContext>>(pipelineService, engineEventHub);
            }

            std::shared_ptr<services::IFrameService<TDataContext>> getFrameService() override
            {
                return std::make_shared<dory::domain::services::BasicFrameService<TDataContext>>();
            }

            std::shared_ptr<TDataContext> getDataContext() override
            {
                return std::make_shared<TDataContext>();
            }

            void configureProject() override
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

                engineEventHub = std::make_shared<events::EngineEventHubDispatcher<TDataContext>>();
                consoleEventHub = std::make_shared<events::SystemConsoleEventHubDispatcher<TDataContext>>();
                glfwWindowEventHub = std::make_shared<events::WindowEventHubDispatcher<TDataContext>>();
            }

            void configurePipeline(TDataContext& context) override
            {
                auto inputGroupNode = pipelineNodeRepository->store(entity::PipelineNode(pipelineNodeIdFactory->generate(), nullptr, 0, entity::nullId, "input group"));
                auto outputGroupNode = pipelineNodeRepository->store(entity::PipelineNode(pipelineNodeIdFactory->generate(), nullptr, 1, entity::nullId, "output group"));
                context.inputGroupNodeId = inputGroupNode.id;
                context.outputGroupNodeId = outputGroupNode.id;

                auto consoleController = std::make_shared<dory::win32::ConsoleController<TDataContext>>(consoleEventHub);
                auto consoleControllerNode = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(pipelineNodeIdFactory->generate(), consoleController, 0, inputGroupNode.id));
                consoleController->initialize(consoleControllerNode.id, context);

                auto windowController = std::make_shared<dory::openGL::GlfwWindowController<TDataContext>>(windowRepositoryReader, glfwWindowEventHub);
                auto windowControllerNode = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(pipelineNodeIdFactory->generate(), windowController, 0, inputGroupNode.id));
                windowController->initialize(windowControllerNode.id, context);
            }

            void attachEventHandlers() override
            {
                engineEventHub->onInitializeEngine() += std::bind(&Project::onInitializeEngine, this, std::placeholders::_1, std::placeholders::_2);
                consoleEventHub->onKeyPressed() += std::bind(&Project::onConsoleKeyPressed, this, std::placeholders::_1, std::placeholders::_2);
                glfwWindowEventHub->onCloseWindow() += std::bind(&Project::onCloseWindow, this, std::placeholders::_1, std::placeholders::_2);
            }

        private:
            entity::IdType newWindow(TDataContext& context)
            {
                dory::openGL::GlfwWindowParameters glfwWindowParameters;
                auto glfwWindowHandler = dory::openGL::GlfwWindowFactory::createWindow(glfwWindowParameters);
                auto window = windowRespository->store(dory::openGL::GlfwWindow(windowIdFactory->generate(), glfwWindowHandler));

                auto camera = cameraRepository->store(dory::domain::entity::Camera(cameraIdFactory->generate()));
                dory::domain::entity::Viewport viewport(0, 0, 0, 0);

                auto viewController = std::make_shared<dory::openGL::ViewControllerOpenGL<TDataContext>>(viewRepositoryReader, configuration, windowRepositoryReader);
                auto viewControllerNode = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(pipelineNodeIdFactory->generate(), 
                    viewController, 0, context.outputGroupNodeId));

                auto view = viewRepository->store(dory::domain::entity::View(viewIdFactory->generate(), window.id, viewControllerNode.id, camera.id, viewport));

                viewController->initialize(viewControllerNode.id, context);

                std::cout << "PipelineNode count: " << pipelineNodeRepository->getEntitiesCount() << std::endl;

                return window.id;
            }

            void onInitializeEngine(TDataContext& context, const events::InitializeEngineEventData& eventData)
            {
                context.mainWindowId = newWindow(context);
            }

            void onConsoleKeyPressed(TDataContext& context, events::KeyPressedEventData& eventData)
            {
                if(eventData.keyPressed == 27)
                {
                    context.isStop = true;
                    std::cout << std::this_thread::get_id() << ": ESC" << std::endl;
                }
                else if(eventData.keyPressed == 119)
                {
                    newWindow(context);
                }
                else if(eventData.keyPressed != 0)
                {
                    std::cout << std::this_thread::get_id() << ": key pressed: " << eventData.keyPressed << std::endl;
                }
            }

            void onCloseWindow(TDataContext& context, events::CloseWindowEventData& eventData)
            {
                auto windowId = eventData.windowId;
                auto window = windowRepositoryReader->get(windowId);

                if(window)
                {
                    auto windowHandler = window->handler;

                    dory::openGL::GlfwWindowFactory::closeWindow(windowHandler);

                    if(windowId == context.mainWindowId)
                    {
                        context.isStop = true;
                    }
                    std::cout << "Close window(id " << windowId << ")" << std::endl;

                    entity::IdType viewControllerNodeId = pipelineNodeIdFactory->getNullId();

                    windowRespository->remove(window);
                    viewRepository->remove([&windowId, &viewControllerNodeId](dory::domain::entity::View& view)
                    {
                        if(view.windowId == windowId)
                        {
                            viewControllerNodeId = view.controllerNodeId;
                            return true;
                        }

                        return false;
                    });

                    if(viewControllerNodeId != pipelineNodeIdFactory->getNullId())
                    {
                        pipelineNodeRepository->remove([&viewControllerNodeId](dory::domain::entity::PipelineNode& node)
                        {
                            return node.id == viewControllerNodeId;
                        });
                    }

                    std::cout << "PipelineNode count: " << pipelineNodeRepository->getEntitiesCount() << std::endl;
                }
            }
    };
}