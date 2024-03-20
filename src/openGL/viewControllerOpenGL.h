#pragma once

#include "glfwWindow.h"
#include "renderer.h"
#include "base/domain/controller.h"
#include "base/domain/entityRepository.h"
#include "base/domain/entity.h"

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

    template<typename TDataContext, typename TRenderer, typename TViewRepository, typename TWindowRepository>
    class ViewControllerOpenGL2: public domain::Controller2<TDataContext>
    {
    private:
        using RendererType = IRenderer<TRenderer>;
        RendererType& renderer;

        using ViewRepositoryType = domain::IEntityRepository<TViewRepository, domain::entity::View, domain::entity::IdType>;
        ViewRepositoryType& viewRepository;

        using WindowRepositoryType = domain::IEntityRepository<TWindowRepository, GlfwWindow, domain::entity::IdType>;
        WindowRepositoryType& windowRepository;

    public:
        explicit ViewControllerOpenGL2(RendererType& renderer, ViewRepositoryType& viewRepository, WindowRepositoryType& windowRepository):
                renderer(renderer),
                viewRepository(viewRepository),
                windowRepository(windowRepository)
        {}

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
            auto view = viewRepository.find([&referenceId](const domain::entity::View& view)
            {
                return view.controllerNodeId == referenceId;
            });

            if(view.has_value())
            {
                auto glfwWindow = windowRepository.get(view->windowId);
                if(glfwWindow.has_value())
                {
                    return glfwWindow->handler;
                }
            }

            return nullptr;
        }
    };
}