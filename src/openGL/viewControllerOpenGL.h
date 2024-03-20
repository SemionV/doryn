#pragma once

#include "glfwWindow.h"
#include "renderer.h"
#include "base/domain/controller.h"

namespace dory::openGL
{
    template<typename TDataContext, typename TServiceLocator>
    class ViewControllerOpenGL: public domain::Controller<TDataContext, TServiceLocator>
    {
    private:
        Renderer<TServiceLocator> renderer;

    public:
        explicit ViewControllerOpenGL(TServiceLocator& serviceLocator):
            domain::Controller<TDataContext, TServiceLocator>(serviceLocator),
            renderer(serviceLocator)
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
                renderer.initialize();
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
                renderer.draw();
                glfwSwapBuffers(windowHandler);
            }
        }

    private:
        GLFWwindow* getWindowHandler(domain::entity::IdType referenceId)
        {
            auto view = this->services.viewRepository.find([&referenceId](const domain::entity::View& view)
            {
                return view.controllerNodeId == referenceId;
            });

            if(view.has_value())
            {
                auto glfwWindow = this->services.windowRepository.get(view->windowId);
                if(glfwWindow.has_value())
                {
                    return glfwWindow->handler;
                }
            }

            return nullptr;
        }
    };

    template<typename TDataContext, typename TServiceLocator>
    class ViewControllerOpenGL2: public domain::Controller<TDataContext, TServiceLocator>
    {
    private:
        Renderer<TServiceLocator> renderer;

    public:
        explicit ViewControllerOpenGL2(TServiceLocator& serviceLocator):
                domain::Controller<TDataContext, TServiceLocator>(serviceLocator),
                renderer(serviceLocator)
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
                renderer.initialize();
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
                renderer.draw();
                glfwSwapBuffers(windowHandler);
            }
        }

    private:
        GLFWwindow* getWindowHandler(domain::entity::IdType referenceId)
        {
            auto view = this->services.viewRepository.find([&referenceId](const domain::entity::View& view)
                                                           {
                                                               return view.controllerNodeId == referenceId;
                                                           });

            if(view.has_value())
            {
                auto glfwWindow = this->services.windowRepository.get(view->windowId);
                if(glfwWindow.has_value())
                {
                    return glfwWindow->handler;
                }
            }

            return nullptr;
        }
    };
}