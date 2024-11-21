#pragma once

#include <dory/engine/controllers/controller.h>
#include "dory/engine/repositories/entityRepository.h"
#include "dory/engine/resources/entity.h"

#include <dory/engine/resources/opengl/glfwWindow.h>
#include <dory/engine/services/opengl/renderer.h>

namespace dory::engine::controllers::opengl
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
    class ViewControllerOpenGL: public Controller<typename T::DataContextType>
    {
    private:
        using RendererType = T::RendererType;
        RendererType renderer;

        using ViewRepositoryType = repositories::IEntityRepository<typename T::ViewRepositoryType, resources::entity::View, resources::entity::IdType>;
        ViewRepositoryType& viewRepository;

        using WindowRepositoryType = repositories::IEntityRepository<typename T::WindowRepositoryType, resources::opengl::GlfwWindow, resources::entity::IdType>;
        WindowRepositoryType& windowRepository;

    public:
        using FactoryType = ViewControllerFactory<T>;

        explicit ViewControllerOpenGL(RendererType renderer, ViewRepositoryType& viewRepository, WindowRepositoryType& windowRepository):
                renderer(renderer),
                viewRepository(viewRepository),
                windowRepository(windowRepository)
        {}

        bool initialize(resources::entity::IdType referenceId, T::DataContextType& context) override
        {
            auto windowHandler = getWindowHandler(referenceId);
            if(windowHandler != nullptr)
            {
                glfwMakeContextCurrent(windowHandler);

                int version = gladLoadGL(glfwGetProcAddress);
                if (version == 0) {
                    return false;
                }

                renderer.initialize(context);
            }

            return true;
        }

        void stop(resources::entity::IdType referenceId, T::DataContextType& context) override
        {
        }

        void update(resources::entity::IdType referenceId, const resources::TimeSpan& timeStep, T::DataContextType& context) override
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
        GLFWwindow* getWindowHandler(resources::entity::IdType referenceId)
        {
            auto view = viewRepository.find([&referenceId](const resources::entity::View& view)
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
        using ControllerInterfaceType = Controller<typename T::DataContextType>;

        using RendererFactoryType = IServiceFactory<typename T::RendererFactoryType>;
        RendererFactoryType& rendererFactory;

        using ViewRepositoryType = repositories::IEntityRepository<typename T::ViewRepositoryType, resources::entity::View, resources::entity::IdType>;
        ViewRepositoryType& viewRepository;

        using WindowRepositoryType = repositories::IEntityRepository<typename T::WindowRepositoryType, resources::opengl::GlfwWindow, resources::entity::IdType>;
        WindowRepositoryType& windowRepository;

    public:
        explicit ViewControllerFactory(RendererFactoryType& rendererFactory, ViewRepositoryType& viewRepository, WindowRepositoryType& windowRepository):
                rendererFactory(rendererFactory),
                viewRepository(viewRepository),
                windowRepository(windowRepository)
        {}

        std::shared_ptr<ControllerInterfaceType> createInstanceImpl()
        {
            return std::static_pointer_cast<ControllerInterfaceType>(std::make_shared<ViewControllerOpenGL<T>>
                (rendererFactory.createInstance(), viewRepository, windowRepository));
        }
    };
}