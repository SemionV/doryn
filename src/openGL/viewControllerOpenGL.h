#pragma once

#include "glfwWindow.h"
#include "renderer.h"

namespace dory::openGL
{
    template<typename TDataContext, typename TServiceLocator>
    class ViewControllerOpenGL: public domain::ViewController<TDataContext, TServiceLocator>
    {
        private:
            std::shared_ptr<domain::RepositoryReader<GlfwWindow>> windowRespository;
            std::shared_ptr<Renderer<TServiceLocator>> renderer;

        public:
            ViewControllerOpenGL(const TServiceLocator& serviceLocator,
                    std::shared_ptr<domain::RepositoryReader<domain::entity::View>> viewRepository,
                    std::shared_ptr<domain::RepositoryReader<GlfwWindow>> windowRespository,
                    std::shared_ptr<Renderer<TServiceLocator>> renderer):
                domain::ViewController<TDataContext, TServiceLocator>(serviceLocator, viewRepository),
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
                    renderer->initialize();
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
                auto view = this->viewRepository->get(referenceId, [](const domain::entity::View& view, domain::entity::IdType referenceId)
                {
                    return view.controllerNodeId == referenceId;
                });

                if(view.has_value())
                {
                    auto glfwWindow = windowRespository->get(view.value().windowId);
                    if(glfwWindow.has_value())
                    {
                        return glfwWindow.value().handler;
                    }
                }

                return nullptr;
            }
    };
}