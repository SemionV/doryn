#pragma once

#include "glfwWindow.h"
#include "renderer.h"
#include "base/domain/controller.h"
#include "base/domain/entityRepository.h"
#include "base/domain/entity.h"

namespace dory::openGL
{
    template<typename T>
    class ViewControllerFactory;

    template<typename TDataContext, typename TRenderer, typename TViewRepository, typename TWindowRepository, typename TRendererFactory>
    struct ViewControllerDependencies
    {
        using DataContextType = TDataContext;
        using RendererType = TRenderer;
        using ViewRepositoryType = TViewRepository;
        using WindowRepositoryType = TWindowRepository;
        using RendererFactoryType = TRendererFactory;
    };

    template<typename T>
    requires(is_instance_v<T, ViewControllerDependencies>)
    class ViewControllerOpenGL2: public domain::Controller2<typename T::DataContextType>
    {
    private:
        using RendererType = T::RendererType;
        RendererType renderer;

        using ViewRepositoryType = domain::IEntityRepository<typename T::ViewRepositoryType, domain::entity::View, domain::entity::IdType>;
        ViewRepositoryType& viewRepository;

        using WindowRepositoryType = domain::IEntityRepository<typename T::WindowRepositoryType, GlfwWindow, domain::entity::IdType>;
        WindowRepositoryType& windowRepository;

    public:
        using FactoryType = ViewControllerFactory<T>;

        explicit ViewControllerOpenGL2(RendererType renderer, ViewRepositoryType& viewRepository, WindowRepositoryType& windowRepository):
                renderer(renderer),
                viewRepository(viewRepository),
                windowRepository(windowRepository)
        {}

        bool initialize(domain::entity::IdType referenceId, T::DataContextType& context) override
        {
            auto windowHandler = getWindowHandler(referenceId);
            if(windowHandler != nullptr)
            {
                glfwMakeContextCurrent(windowHandler);
                gl3wInit();
                renderer.initialize();
            }

            return true;
        }

        void stop(domain::entity::IdType referenceId, T::DataContextType& context) override
        {
        }

        void update(domain::entity::IdType referenceId, const domain::TimeSpan& timeStep, T::DataContextType& context) override
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

    template<typename T>
    class ViewControllerFactory: public IServiceFactory<ViewControllerFactory<T>>
    {
    private:
        using ControllerInterfaceType = domain::Controller2<typename T::DataContextType>;

        using RendererFactoryType = IServiceFactory<typename T::RendererFactoryType>;
        RendererFactoryType& rendererFactory;

        using ViewRepositoryType = domain::IEntityRepository<typename T::ViewRepositoryType, domain::entity::View, domain::entity::IdType>;
        ViewRepositoryType& viewRepository;

        using WindowRepositoryType = domain::IEntityRepository<typename T::WindowRepositoryType, GlfwWindow, domain::entity::IdType>;
        WindowRepositoryType& windowRepository;

    public:
        explicit ViewControllerFactory(RendererFactoryType& rendererFactory, ViewRepositoryType& viewRepository, WindowRepositoryType& windowRepository):
                rendererFactory(rendererFactory),
                viewRepository(viewRepository),
                windowRepository(windowRepository)
        {}

        std::shared_ptr<ControllerInterfaceType> createInstanceImpl()
        {
            return std::static_pointer_cast<ControllerInterfaceType>(std::make_shared<ViewControllerOpenGL2<T>>
                (rendererFactory.createInstance(), viewRepository, windowRepository));
        }
    };
}