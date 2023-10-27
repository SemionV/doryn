#pragma once

#include "glfwWindow.h"
#include "renderer.h"

namespace dory::openGL
{
    template<class TDataContext>
    class ViewControllerOpenGL: public domain::ViewController<TDataContext, GlfwWindow>
    {
        private:
            std::shared_ptr<domain::RepositoryReader<GlfwWindow>> windowRespository;
            std::shared_ptr<Renderer> renderer;

        public:
            ViewControllerOpenGL(std::shared_ptr<domain::RepositoryReader<domain::entity::View>> viewRepository, 
                    std::shared_ptr<configuration::IConfiguration> configuration,
                    std::shared_ptr<domain::RepositoryReader<GlfwWindow>> windowRespository,
                    std::shared_ptr<Renderer> renderer):
                domain::ViewController<TDataContext, GlfwWindow>(viewRepository, configuration),
                windowRespository(windowRespository),
                renderer(renderer)
            {
            }

            bool initialize(domain::entity::IdType referenceId, TDataContext& context) override
            {
                std::cout << "initialize: OpenGL Basic View" << std::endl;

                auto windowHandler = getWindowHandler(referenceId);
                if(windowHandler != nullptr)
                {
                    glfwMakeContextCurrent(windowHandler);
                    gl3wInit();
                    renderer->initialize(this->configuration);
                }

                return true;
            }

            void stop(domain::entity::IdType referenceId, TDataContext& context) override
            {

            }

            void update(domain::entity::IdType referenceId, const domain::TimeSpan& timeStep, TDataContext& context) override
            {
                auto windowHandler = getWindowHandler(referenceId);
                if(windowHandler != nullptr)
                {
                    glfwMakeContextCurrent(windowHandler);
                    renderer->draw();
                    glfwSwapBuffers(windowHandler);
                }
            }

        private:
            GLFWwindow* getWindowHandler(domain::entity::IdType referenceId)
            {
                auto view = this->viewRepository->get(referenceId, [](domain::entity::View* view, domain::entity::IdType referenceId)
                {
                    return view->controllerNodeId == referenceId;
                });

                if(view != nullptr)
                {
                    auto glfwWindow = windowRespository->get(view->windowId);
                    if(glfwWindow)
                    {
                        return glfwWindow->handler;
                    }
                }

                return nullptr;
            }
    };
}