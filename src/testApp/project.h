#pragma once

#include "serviceLocator.h"
#include "projectDataContext.h"

namespace testApp
{
    class Project: dory::domain::Service<ServiceLocator>
    {
        public:
            void run(ProjectDataContext& context)
            {
                attachEventHandlers();

                this->services.engine.initialize(context);
                this->services.frameService.startLoop(context);
            }

        protected:
            void attachEventHandlers()
            {
                this->services.engineEventHub.onInitializeEngine().attachHandler(this, &Project::onInitializeEngine);
                this->services.engineEventHub.onStopEngine().attachHandler(this, &Project::onStopEngine);

                this->services.consoleEventHub.onKeyPressed().attachHandler(this, &Project::onConsoleKeyPressed);
                this->services.getGlfwWindowEventHub()->onCloseWindow().attachHandler(this, &Project::onCloseWindow);
            }
        
        private:
            auto newWindow(ProjectDataContext& context)
            {
                auto windowRespository = this->services.getWindowRepository();
                auto windowIdFactory = this->services.getWindowIdFactory();
                dory::openGL::GlfwWindowParameters glfwWindowParameters;
                auto glfwWindowHandler = dory::openGL::GlfwWindowFactory::createWindow(glfwWindowParameters);
                auto window = windowRespository->store(dory::openGL::GlfwWindow(windowIdFactory->generate(), glfwWindowHandler));

                auto camera = dory::domain::entity::Camera(this->services.cameraIdFactory.generate());
                this->services.cameraRepository.store(camera);

                auto windowRepositoryReader = this->services.getWindowRepositoryReader();
                auto renderer = std::make_shared<dory::openGL::Renderer<ServiceLocator>>(this->services);
                auto viewController = std::make_shared<dory::openGL::ViewControllerOpenGL<ProjectDataContext, ServiceLocator>>(this->services, windowRepositoryReader, renderer);
                auto viewControllerNode = dory::domain::entity::PipelineNode(this->services.pipelineNodeIdFactory.generate(),
                                                                             viewController,
                                                                             dory::domain::entity::PipelineNodePriority::Default,
                                                                             context.outputGroupNodeId);

                this->services.pipelineNodeRepository.store(viewControllerNode);

                auto viewport = dory::domain::entity::Viewport(0, 0, 0, 0);
                auto view = dory::domain::entity::View(this->services.viewIdFactory.generate(), window.id, viewControllerNode.id, camera.id, viewport);
                this->services.viewRepository.store(view);

                viewController->initialize(viewControllerNode.id, context);

                std::cout << "PipelineNode count: " << this->services.pipelineNodeRepository.count() << std::endl;

                return window.id;
            }

            void configurePipeline(ProjectDataContext& context)
            {
                auto inputGroupNode = dory::domain::entity::PipelineNode(this->services.pipelineNodeIdFactory.generate(),
                                                                         nullptr,
                                                                         dory::domain::entity::PipelineNodePriority::Default,
                                                                         dory::entity::nullId,
                                                                         "input group");
                this->services.pipelineNodeRepository.store(inputGroupNode);

                auto outputGroupNode = dory::domain::entity::PipelineNode(this->services.pipelineNodeIdFactory.generate(),
                                                                          nullptr,
                                                                          dory::domain::entity::PipelineNodePriority::First,
                                                                          dory::entity::nullId,
                                                                          "output group");
                this->services.pipelineNodeRepository.store(outputGroupNode);

                context.inputGroupNodeId = inputGroupNode.id;
                context.outputGroupNodeId = outputGroupNode.id;

                auto consoleController = std::make_shared<dory::win32::ConsoleController<ProjectDataContext>>(consoleEventHub);
                auto consoleControllerNode = dory::domain::entity::PipelineNode(this->services.pipelineNodeIdFactory.generate(),
                                                                                consoleController,
                                                                                dory::domain::entity::PipelineNodePriority::Default,
                                                                                inputGroupNode.id);
                this->services.pipelineNodeRepository.store(consoleControllerNode);
                consoleController->initialize(consoleControllerNode.id, context);

                auto windowRepositoryReader = this->services.getWindowRepositoryReader();
                auto glfwWindowEventHub = this->services.getGlfwWindowEventHub();

                auto windowController = std::make_shared<dory::openGL::GlfwWindowController<ProjectDataContext>>(windowRepositoryReader, glfwWindowEventHub);
                auto windowControllerNode = dory::domain::entity::PipelineNode(this->services.pipelineNodeIdFactory.generate(),
                                                                               windowController,
                                                                               dory::domain::entity::PipelineNodePriority::Default,
                                                                               inputGroupNode.id);
                this->services.pipelineNodeRepository.store(windowControllerNode);
                windowController->initialize(windowControllerNode.id, context);
            }

            void onInitializeEngine(ProjectDataContext& context, const events::InitializeEngineEventData& eventData)
            {
                std::cout << "Starting Engine..." << std::endl;

                configurePipeline(context);
                context.mainWindowId = newWindow(context);
            }

            void onStopEngine(ProjectDataContext& context, const events::StopEngineEventData& eventData)
            {
                std::cout << "Stopping Engine..." << std::endl;
            }

            void onConsoleKeyPressed(ProjectDataContext& context, events::KeyPressedEventData& eventData)
            {
                if(eventData.keyPressed == 27)
                {
                    this->services.frameService.endLoop();
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

            void onCloseWindow(ProjectDataContext& context, events::CloseWindowEventData& eventData)
            {
                auto windowId = eventData.windowId;
                auto windowRepositoryReader = this->services.getWindowRepositoryReader();
                auto window = windowRepositoryReader->get(windowId);

                if(window.has_value())
                {
                    auto windowHandler = window->handler;

                    dory::openGL::GlfwWindowFactory::closeWindow(windowHandler);

                    if(windowId == context.mainWindowId)
                    {
                        this->services.frameService.endLoop();
                    }
                    std::cout << "Close window(id " << windowId << ")" << std::endl;

                    auto& pipelineNodeIdFactory = this->services.pipelineNodeIdFactory;
                    auto viewControllerNodeId = pipelineNodeIdFactory.getNullId();

                    auto windowRepository = this->services.getWindowRepository();
                    windowRepository->remove(*window);

                    auto view = this->services.viewRepository.find([&windowId](const dory::domain::entity::View& view)
                    {
                        return view.windowId == windowId;
                    });

                    if(view.has_value())
                    {
                        viewControllerNodeId = view->controllerNodeId;
                        this->services.viewRepository.remove(view->id);
                    }

                    if(viewControllerNodeId != pipelineNodeIdFactory.getNullId())
                    {
                        this->services.pipelineNodeRepository.remove(viewControllerNodeId);
                    }

                    std::cout << "PipelineNode count: " << this->services.pipelineNodeRepository.count() << std::endl;
                }
            }
    };
}