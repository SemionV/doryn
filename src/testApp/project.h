#pragma once

#include "serviceLocator.h"

namespace testApp
{
    template<class TDataContext>
    class Project
    {
        using ServiceLocator = ServiceLocator<TDataContext>;

        private:
            //TODO: convert shared_ptr to const &
            std::shared_ptr<ServiceLocator> serviceLocator;

        public:
            Project(std::shared_ptr<ServiceLocator> serviceLocator): serviceLocator(serviceLocator)
            {}

            void run(TDataContext& context)
            {
                attachEventHandlers();

                auto pipelineService = serviceLocator->getPipelineService();
                auto engineEventHub = serviceLocator->getEngineEventHub();
                dory::domain::Engine<TDataContext> engine(pipelineService, engineEventHub);
                engine.initialize(context);

                auto frameService = serviceLocator->getFrameService();
                frameService->startLoop(engine, context);
            }

        protected:
            void attachEventHandlers()
            {
                serviceLocator->getEngineEventHub()->onInitializeEngine().attachHandler(this, &Project::onInitializeEngine); 
                serviceLocator->getEngineEventHub()->onStopEngine().attachHandler(this, &Project::onStopEngine); 
                
                serviceLocator->getConsoleEventHub()->onKeyPressed().attachHandler(this, &Project::onConsoleKeyPressed);
                serviceLocator->getGlfwWindowEventHub()->onCloseWindow().attachHandler(this, &Project::onCloseWindow);
            }
        
        private:
            auto newWindow(TDataContext& context)
            {
                auto windowRespository = serviceLocator->getWindowRepository();
                auto windowIdFactory = serviceLocator->getWindowIdFactory();
                dory::openGL::GlfwWindowParameters glfwWindowParameters;
                auto glfwWindowHandler = dory::openGL::GlfwWindowFactory::createWindow(glfwWindowParameters);
                auto window = windowRespository->store(dory::openGL::GlfwWindow(windowIdFactory->generate(), glfwWindowHandler));

                auto cameraRepository = serviceLocator->getCameraRepository();
                auto cameraIdFactory = serviceLocator->getCameraIdFactory();
                auto camera = cameraRepository->store(dory::domain::entity::Camera(cameraIdFactory->generate()));
                dory::domain::entity::Viewport viewport(0, 0, 0, 0);

                auto pipelineNodeIdFactory = serviceLocator->getPipelineNodeIdFactory();
                auto pipelineNodeRepository = serviceLocator->getPipelineNodeRepository();
                auto viewRepositoryReader = serviceLocator->getViewRepositoryReader();
                auto windowRepositoryReader = serviceLocator->getWindowRepositoryReader();
                auto renderer = std::make_shared<dory::openGL::Renderer<ServiceLocator>>(*serviceLocator);
                auto viewController = std::make_shared<dory::openGL::ViewControllerOpenGL<TDataContext, ServiceLocator>>(*serviceLocator, viewRepositoryReader, windowRepositoryReader, renderer);
                auto viewControllerNode = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(pipelineNodeIdFactory->generate(), 
                    viewController, 0, context.outputGroupNodeId));

                auto viewIdFactory = serviceLocator->getViewIdFactory();
                auto viewRepository = serviceLocator->getViewRepository();
                viewRepository->store(dory::domain::entity::View(viewIdFactory->generate(), window.id, viewControllerNode.id, camera.id, viewport));

                viewController->initialize(viewControllerNode.id, context);

                std::cout << "PipelineNode count: " << pipelineNodeRepository->getEntitiesCount() << std::endl;

                return window.id;
            }

            void configurePipeline(TDataContext& context)
            {
                auto pipelineNodeIdFactory = serviceLocator->getPipelineNodeIdFactory();
                auto pipelineNodeRepository = serviceLocator->getPipelineNodeRepository();
                auto inputGroupNode = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(pipelineNodeIdFactory->generate(), nullptr, 0, dory::entity::nullId, "input group"));
                auto outputGroupNode = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(pipelineNodeIdFactory->generate(), nullptr, 1, dory::entity::nullId, "output group"));
                context.inputGroupNodeId = inputGroupNode.id;
                context.outputGroupNodeId = outputGroupNode.id;

                auto consoleEventHub = serviceLocator->getConsoleEventHub();
                auto consoleController = std::make_shared<dory::win32::ConsoleController<TDataContext>>(consoleEventHub);
                auto consoleControllerNode = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(pipelineNodeIdFactory->generate(), consoleController, 0, inputGroupNode.id));
                consoleController->initialize(consoleControllerNode.id, context);

                auto windowRepositoryReader = serviceLocator->getWindowRepositoryReader();
                auto glfwWindowEventHub = serviceLocator->getGlfwWindowEventHub();

                auto windowController = std::make_shared<dory::openGL::GlfwWindowController<TDataContext>>(windowRepositoryReader, glfwWindowEventHub);
                auto windowControllerNode = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(pipelineNodeIdFactory->generate(), windowController, 0, inputGroupNode.id));
                windowController->initialize(windowControllerNode.id, context);
            }

            void onInitializeEngine(TDataContext& context, const events::InitializeEngineEventData& eventData)
            {
                std::cout << "Starting Engine..." << std::endl;

                configurePipeline(context);
                context.mainWindowId = newWindow(context);
            }

            void onStopEngine(TDataContext& context, const events::StopEngineEventData& eventData)
            {
                std::cout << "Stopping Engine..." << std::endl;
            }

            void onConsoleKeyPressed(TDataContext& context, events::KeyPressedEventData& eventData)
            {
                if(eventData.keyPressed == 27)
                {
                    serviceLocator->getFrameService()->endLoop();
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
                auto windowRepositoryReader = serviceLocator->getWindowRepositoryReader();
                auto window = windowRepositoryReader->get(windowId);

                if(window.has_value())
                {
                    auto windowHandler = window.value().handler;

                    dory::openGL::GlfwWindowFactory::closeWindow(windowHandler);

                    if(windowId == context.mainWindowId)
                    {
                        serviceLocator->getFrameService()->endLoop();
                    }
                    std::cout << "Close window(id " << windowId << ")" << std::endl;

                    auto pipelineNodeIdFactory = serviceLocator->getPipelineNodeIdFactory();
                    auto viewControllerNodeId = pipelineNodeIdFactory->getNullId();

                    auto viewRepository = serviceLocator->getViewRepository();
                    auto windowRepository = serviceLocator->getWindowRepository();
                    windowRepository->remove(window.value());
                    viewRepository->remove([&windowId, &viewControllerNodeId](const dory::domain::entity::View& view)
                    {
                        if(view.windowId == windowId)
                        {
                            viewControllerNodeId = view.controllerNodeId;
                            return true;
                        }

                        return false;
                    });

                    auto pipelineNodeRepository = serviceLocator->getPipelineNodeRepository();
                    if(viewControllerNodeId != pipelineNodeIdFactory->getNullId())
                    {
                        pipelineNodeRepository->remove([&viewControllerNodeId](const dory::domain::entity::PipelineNode& node)
                        {
                            return node.id == viewControllerNodeId;
                        });
                    }

                    std::cout << "PipelineNode count: " << pipelineNodeRepository->getEntitiesCount() << std::endl;
                }
            }
    };
}