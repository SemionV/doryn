#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "dory/engine/resources/entity.h"
#include "dory/engine/events/eventTypes.h"
#include <dory/engine/controllers/controller.h>
#include "dory/engine/repositories/entityRepository.h"
#include <dory/engine/resources/opengl/glfwWindow.h>

namespace dory::engine::controllers::opengl
{
    template<class TDataContext, typename TWindowRepository>
    class GlfwWindowControllerFactory;

    template<class TDataContext, typename TWindowRepository>
    class GlfwWindowController: public Controller<TDataContext>
    {
    private:
        using WindowRepositoryType = repositories::IEntityRepository<TWindowRepository, resources::opengl::GlfwWindow, resources::entity::IdType>;
        WindowRepositoryType& windowRepository;

        using WindowEventHubType = events::window::Dispatcher<TDataContext>;
        WindowEventHubType& windowEventHubDispatcher;

    public:
        using FactoryType = GlfwWindowControllerFactory<TDataContext, TWindowRepository>;

        explicit GlfwWindowController(WindowRepositoryType& windowRepository,
                                      WindowEventHubType& windowEventHubDispatcher):
                windowRepository(windowRepository),
                windowEventHubDispatcher(windowEventHubDispatcher)
        {}

        bool initialize(resources::entity::IdType referenceId, TDataContext& context) override
        {
            return glfwInit();
        };

        void stop(resources::entity::IdType referenceId, TDataContext& context) override
        {
            glfwTerminate();
        }

        void update(resources::entity::IdType referenceId, const resources::TimeSpan& timeStep, TDataContext& context) override
        {
            glfwPollEvents();

            windowRepository.forEach([this](auto& window)
            {
                if(glfwWindowShouldClose(window.handler))
                {
                    this->windowEventHubDispatcher.charge(events::window::Close(window.id));
                    glfwSetWindowShouldClose(window.handler, 0);
                }
            });

            windowEventHubDispatcher.fireAll(context);
        }
    };

    template<class TDataContext, typename TWindowRepository>
    class GlfwWindowControllerFactory: public IServiceFactory<GlfwWindowControllerFactory<TDataContext, TWindowRepository>>
    {
    private:
        using ControllerInterfaceType = Controller<TDataContext>;

        using WindowRepositoryType = repositories::IEntityRepository<TWindowRepository, resources::opengl::GlfwWindow, resources::entity::IdType>;
        WindowRepositoryType& windowRepository;

        using WindowEventHubType = events::window::Dispatcher<TDataContext>;
        WindowEventHubType& windowEventHubDispatcher;

    public:
        explicit GlfwWindowControllerFactory(WindowRepositoryType& windowRepository,
                                             WindowEventHubType& windowEventHubDispatcher):
                windowRepository(windowRepository),
                windowEventHubDispatcher(windowEventHubDispatcher)
        {}

        std::shared_ptr<ControllerInterfaceType> createInstanceImpl()
        {
            return std::static_pointer_cast<ControllerInterfaceType>(std::make_shared<GlfwWindowController<TDataContext, TWindowRepository>>(windowRepository, windowEventHubDispatcher));
        }
    };
}